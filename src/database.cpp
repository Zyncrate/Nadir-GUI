// material data would be loaded in here 

#include "../include/globals.h"
#include "../include/sqlite3.h"
#include <vector>
#include <iostream>
#include <string>



std::vector<Material>allMaterials; 
std::vector<Material>Metals;
std::vector<Material>Ceramic;
std::vector<Material>Polymer;
std::vector<Material>Composite;
std::vector<Material>Semiconductor;
std::vector<Material>Natural;
	

void loadDataFromDB(){
	sqlite3* db = nullptr;
	int result = sqlite3_open("materials.db", &db);
	if (result!=SQLITE_OK){
		std::cerr<<"Could not open database: "<<sqlite3_errmsg(db)<<std::endl;
		return;
	}
	std::cout << "Database opened."<<std::endl;

	const char* selectSQL = "SELECT Material_Name, Material_Family, Density_kg_m3, Thermal_Conductivity_W_mK,Specific_Heat_J_kgK,Thermal_Expansion_1e6_K, Melting_Point_C, Refractive_Index, Transparency,Corrosion_Resistance, Youngs_Modulus_GPa,Tensile_Strength_MPa, Hardness_Mohs, Poissons_Ratio, Electrical_Resistivity_Ohm_m, Dielectric_Constant,pH_Min,pH_Max FROM materials_properties";

	sqlite3_stmt* stmt = nullptr;
	result=sqlite3_prepare_v2(db,selectSQL,-1,&stmt,nullptr);
	if (result!=SQLITE_OK)
	{
		std::cerr<<"Error Preparing Query:  "<<sqlite3_errmsg(db)<<std::endl;
		sqlite3_close(db);
		return;
	}

	while(sqlite3_step(stmt)==SQLITE_ROW){

		
		std::string name=reinterpret_cast<const char*>(sqlite3_column_text(stmt,0));// method to use for strings if the column has strings
		std::string family= reinterpret_cast<const char*>((const char*)sqlite3_column_text(stmt,1));

		MechanicalProps mech;
		mech.density=sqlite3_column_double(stmt,2);
		mech.modulus=sqlite3_column_double(stmt,10);
		mech.tensileStrength=sqlite3_column_double(stmt,11);
		mech.hardness=sqlite3_column_double(stmt,12);
		mech.p_ratio=sqlite3_column_double(stmt,13);

		ThermalProps therm;
		therm.conductivity=sqlite3_column_double(stmt,3);
		therm.meltingpoint=sqlite3_column_double(stmt,6);
		therm.expansion=sqlite3_column_double(stmt,5);
		therm.heatcapacity=sqlite3_column_double(stmt,4);

		ElectricalProps elec;
		elec.d_constant=sqlite3_column_double(stmt,15);
		elec.resistivity=sqlite3_column_double(stmt,14);
		
		OpticalProps optics;
		optics.transparency=reinterpret_cast<const char*>(sqlite3_column_text(stmt,8));
		optics.refractive_index=sqlite3_column_double(stmt,7);
		
		ChemicalProps chem;
		chem.corrosion_resistance=reinterpret_cast<const char*>(sqlite3_column_text(stmt,9));
		chem.ph_min=sqlite3_column_double(stmt,16);
		chem.ph_max=sqlite3_column_double(stmt,17);

		allMaterials.emplace_back(name,family,mech,elec,chem,therm,optics);
	}
	sqlite3_finalize(stmt); //frees memory
	std::cout<<"Loaded "<<allMaterials.size()<<" materials.\n"<<std::endl;

}


void Categorize(){

	for(const auto& material:allMaterials){
		if (material.family=="Metal") Metals.push_back(material);
		else if(material.family=="Ceramic") Ceramic.push_back(material);
		else if(material.family=="Polymer") Polymer.push_back(material);
		else if(material.family=="Composite") Composite.push_back(material);
		else if(material.family=="Semiconductor") Semiconductor.push_back(material);
		else if(material.family=="Natural") Natural.push_back(material);
	}
}
