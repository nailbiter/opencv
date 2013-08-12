#include "test_precomp.hpp"
#include <cstdlib>
#include <cmath>
#include <algorithm>

class oldSphereF:public cv::optim::Solver::Function{
public:
    double calc(const double* x)const{
        return x[0]*x[0]+x[1]*x[1];
    }
};

TEST(Optim_PFSolver, regression_basic){
    cv::Ptr<cv::optim::PFSolver> solver;
    int error_code=0;
    try{
        solver=cv::optim::createPFSolver(cv::Ptr<cv::optim::Solver::Function>(new oldSphereF()));
    }
    catch(cv::Exception e){
        error_code=e.code;
    }
    CV_Assert(error_code==-215 && solver.empty());

    solver=cv::optim::createPFSolver();
#if 1
#endif
}
