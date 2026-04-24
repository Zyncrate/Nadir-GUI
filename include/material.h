// datastructure of materials 
#pragma once 
#include <string>
#include <vector>

struct MechanicalProps{
	double density;
	double modulus;
	double tensileStrength;
	double hardness;
	double p_ratio;//poissons_ratio
};

struct ElectricalProps{
	double d_constant;//dielectric constant
	double resistivity;
};

struct ThermalProps{
	double conductivity;
	double meltingpoint;
	double expansion;
	double heatcapacity;
};

struct ChemicalProps{
	std::string corrosion_resistance;
	double ph_min;
	double ph_max;
};

struct OpticalProps{
	double refractive_index;
	std::string transparency;
};

/*enum class Family{Metal,Ceramic,Polymer,Composite,Semiconductor,Natural};
*/
class Material{
public:
	std::string name;
	std::string family;
	MechanicalProps mech;
	ElectricalProps elec;
	ChemicalProps chem;
	ThermalProps therm;
	OpticalProps optics;
	double performance_score=0.0;

	//constructor 
	Material(std::string name, std::string family, MechanicalProps mech,ElectricalProps elec,
		ChemicalProps chem,ThermalProps therm,OpticalProps optics): name(name), family(family), mech(mech),elec(elec),chem(chem),therm(therm),optics(optics){};


};

struct t_HC{
	std::string symbol;
	std::string property_name;
	double value;
	std::string string_value;
};

struct t_SC{
	std::string property_name;
	double value;
};



