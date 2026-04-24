#pragma once
#include "material.h"
#include <vector>
#include <string>
#include <map>
#include <variant>

//FIXED DATA
extern std::vector<Material>allMaterials;
extern std::vector<Material>Metals;
extern std::vector<Material>Ceramic;
extern std::vector<Material>Polymer;
extern std::vector<Material>Composite;
extern std::vector<Material>Semiconductor;
extern std::vector<Material>Natural;
extern std::vector<t_HC>Hard_Constraints;
extern std::vector<t_SC>Soft_Constraints;
extern std::vector<Material>Filtered_Materials;


//RUNTIME DATA
/*inline std::map<std::string,double> corrosion_resistance_lookup = {
	{"Poor",0.0},
	{"Good",0.25},
	{"Moderate",0.5},
	{"Excellent",1.0}
};

inline std::map<std::string,double> transparency_lookup = {
	{"Opaque",0.0},
	{"Translucent",1.0}
};*/

 