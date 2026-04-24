#include "../include/globals.h"
#include <vector>
#include <map>
#include <iostream>
#include <string>

// filtering and ranking logic 


/*function filters using hardconstraints 
and paases to a vector(selected materials)*/


double Ashby_Perfomance_Math_Maximize(double property_constraint, double shape_exponent, double objective);
double Ashby_Perfomance_Math_Minimize(double property_constraint, double shape_exponent, double objective);
std::vector<t_HC>Hard_Constraints;
std::vector<t_SC>Soft_Constraints;
std::vector<Material>Filtered_Materials;
std::vector<std::string>Beneficial_property;
std::vector<std::string>NonBeneficial_property;


void Filter(std::vector<Material>&allMaterials, std::vector<t_HC>&Hard_Constraints){
	for (const auto &material:allMaterials)
	{
		bool passes=true;
		std::map<std::string,std::variant<double,std::string>>Material_Properties_Lookup={
			{"density",material.mech.density},
			{"modulus",material.mech.modulus},
			{"tensile-strength",material.mech.tensileStrength},
			{"hardness",material.mech.hardness},
			{"poisson-ratio",material.mech.p_ratio},
			{"dielectric-constant",material.elec.d_constant},
			{"resistivity",material.elec.resistivity},
			{"thermal-conductivity",material.therm.conductivity},
			{"melting-point",material.therm.meltingpoint},
			{"thermal-expansion",material.therm.expansion},
			{"heat-capacity",material.therm.heatcapacity},
			{"corrosion-resistance",material.chem.corrosion_resistance},
			{"minimum-ph",material.chem.ph_min},
			{"maximum-ph",material.chem.ph_max},
			{"refractive-index",material.optics.refractive_index},
			{"transparency",material.optics.transparency}
		};

		for (const auto &constraint:Hard_Constraints){
			auto it=Material_Properties_Lookup.find(constraint.property_name);
			if(it!=Material_Properties_Lookup.end()){
				auto Mat_val=it->second;

				bool Conditionmet=false;
				if(std::holds_alternative<double>(Mat_val)){
					double matD=std::get<double>(Mat_val);
					if(constraint.symbol=="<") Conditionmet=matD<constraint.value;
					else if(constraint.symbol==">") Conditionmet=matD>constraint.value;
					else if(constraint.symbol==">=") Conditionmet=matD>=constraint.value;
					else if(constraint.symbol=="<=") Conditionmet=matD<=constraint.value;
					else if(constraint.symbol=="==") Conditionmet=matD==constraint.value;
				}else if(std::holds_alternative<std::string>(Mat_val)){
					std::string matS=std::get<std::string>(Mat_val);
					Conditionmet=(matS==constraint.string_value);
				}
				if(!Conditionmet){
					passes=false;
					break;
				}
			}
		}
		if(passes)Filtered_Materials.push_back(material);

	}
	
}


//Ashby-Performance Indices
void Selection_Method2(std::vector<Material>&Filtered_Materials,std::string objectives,std::string geometry,std::string constraint,bool option){
	double constraint_value=0.0;
	double objectives_value=0.0;
	double geometry_value=0.0;
	double result=0.0;

	std::map<std::string,double>shape_exponent_map={
		{"tension",1.0},
		{"bending",0.5},
		{"plate",0.33},
		{"torsion",0.5}
	};

	for(auto &material:Filtered_Materials){

		std::map<std::string, double> property_map={
			{"stiffness",material.mech.modulus},
			{"toughness",material.mech.hardness},
			{"thermal-conductivity",material.therm.conductivity},
			{"electrical-resistivity",material.elec.resistivity}
		};
		std::map<std::string,double>objectives_map={
			{"mass",material.mech.density}
		};

		auto it1=property_map.find(constraint);
		auto it2=shape_exponent_map.find(geometry);
		auto it3=objectives_map.find(objectives);
		if(it1!=property_map.end()&&it2!=shape_exponent_map.end()&&it3!=objectives_map.end()){
			constraint_value=it1->second;
			geometry_value=it2->second;
			objectives_value=it3->second;
			double score=0.0;

			if(option){  //true is maximize
				result=Ashby_Perfomance_Math_Maximize(constraint_value,geometry_value,objectives_value);
			}else{
				result=Ashby_Perfomance_Math_Minimize(constraint_value,geometry_value,objectives_value);
			}
		}

		material.performance_score=result; // save performace score
	}

}