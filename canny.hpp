#ifndef CANNY_HPP
#define CANNY_HPP

#include "HashMap.hpp"
#include "global.hpp"

void magnitude_matrix(double **pic, double **mag, double **x, double **y);

std::vector<Point*> peak_detection(double **mag, HashMap *peaks, double **x, double **y);

void recursiveDT(double **mag, double **final, HashMap *h, HashMap *peaks, int i, int j, int flag);

#endif