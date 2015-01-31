#include "uwesub_svs/calibrate_covariance.hpp"
#include <math.h>
namespace calibration {

calib_covariance::calib_covariance(void) {

    covariance_found = false;
    calibration_length = 200;
    calibration_counter = 0;
    variance = 0;
    variance_data.clear();
    variance_data.reserve(calibration_length);
}

calib_covariance::calib_covariance(int cali_length) {

    covariance_found = false;
    calibration_length = cali_length;
    calibration_counter = 0;
    variance = 0;
    variance_data.clear();
    variance_data.reserve(calibration_length);    
}

calib_covariance::~calib_covariance(){

}

/** mean(): calculates the mean
*/
double calib_covariance::mean(const std::vector<double> vec) {
    double sum = 0;
    unsigned int size = 0;
    for (typename std::vector<double>::const_iterator it = vec.begin(); it!=vec.end(); ++it, ++size) 
    { 
        sum += (*it);
        // if size is not == 0, divide by size, else return 0
    }

    return (size)?(sum/(double) size):0;
}

/** mean(): calculates the mean
*/
double calib_covariance::mean(const std::vector<double> vec, typename std::vector<double>::const_iterator start, typename std::vector<double>::const_iterator finish) {
    double sum = 0;
    unsigned int size = 0;
    for (typename std::vector<double>::const_iterator it = start; it!=finish; ++it, ++size) 
    { 
        sum += (*it);
        // if size is not == 0, divide by size, else return 0
    }
    return (size)?(sum/(double) size):0;
}

/** std2(): find the std_deviation across our current_range
*/
double calib_covariance::std2(const std::vector<double> vec, const double mean) {
    double sum = 0;
    unsigned int size = 0;
    for(typename std::vector<double>::const_iterator it = vec.begin(); it != vec.end(); ++it) {
        sum+=pow(((*it) - mean),2);
         ++size;
    }
    // if size is not == 0, divide by size, else return 0            
    return size?sqrt(sum/(double)size):-1;
    
}

void calib_covariance::put_in(double fresh_data) {
    variance_data.push_back(fresh_data);
    calibration_counter ++;

    if (calibration_counter >= calibration_length) {
        offset = mean(variance_data);
        variance = pow( std2(variance_data, offset ), 2);
        
        covariance_found = true;
    }
}

int calib_covariance::get_variance(double & data) {
    if (calibration_counter >= calibration_length) {
       data = variance;
        return EXIT_SUCCESS;
    }
    return EXIT_FAILURE;
}
int calib_covariance::get_offset(double & data) {
    if (calibration_counter >= calibration_length) {
       data = offset;
        return EXIT_SUCCESS;
    }
    return EXIT_FAILURE;
}
 
bool calib_covariance::are_you_calibrated(void) {
   
    return covariance_found;
}

} // end of namespace