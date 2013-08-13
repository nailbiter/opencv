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
class outerSphereF:public cv::optim::PFSolver::Function{
public:
    double calc(const double* x)const{
        if(x){
        return x[0]*x[0]+x[1]*x[1];
        }else{return 0.0;}
    }
};

TEST(Optim_pfsolver, regression_basic){
    //cv::optim::test();
    cv::Ptr<cv::optim::PFSolver> solver;
#if 1
    int error_code=0;
    try{
        cv::optim::Solver::Function *ptr_f=new oldSphereF();
        printf("here is a pointer originally: %p\n",ptr_f);
        solver=cv::optim::createPFSolver(cv::Ptr<cv::optim::Solver::Function>(ptr_f));
    }
    catch(cv::Exception e){
        error_code=e.code;
    }
    CV_Assert(error_code==-215 && solver.empty());
    printf("\terror is correctly generated\n");
#endif
#if 1
    solver=cv::optim::createPFSolver();
    cv::Mat x=(cv::Mat_<double>(1,2)<<1.0,1.0),
        std=(cv::Mat_<double>(1,2)<<2.0,2.0);
    cv::optim::Solver::Function* ptr_f=new outerSphereF();
    solver->setFunction(cv::Ptr<cv::optim::Solver::Function>(ptr_f));
    solver->setParamsSTD(std);
    solver->minimize(x);
    while(solver->iteration()<(solver->getTermCriteria()).maxCount){
        solver->getOptParam(x);
        std::cout<<"x: "<<x<<std::endl;
    }
#endif
}
