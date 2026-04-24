// raw formulas and ashby indices math
/*calculation functions take reside here*/
#include <cmath>
#include <vector>
#include "../include/globals.h"



/*void WPI(std::vector<Material>allMaterials){

}*/


double Ashby_Perfomance_Math_Maximize(double property_constraint, double shape_exponent, double objective){

	auto Score= objective/std::pow(property_constraint,shape_exponent);
	return Score;
}



double Ashby_Perfomance_Math_Minimize(double property_constraint, double shape_exponent,double objective){
	auto Score=std::pow(property_constraint,shape_exponent)/objective;
	return Score;
}
