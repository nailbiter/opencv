#include "precomp.hpp"
#include "debug.hpp"

namespace cv{namespace optim{
    class CV_EXPORTS PFSolverImpl : public PFSolver{
    public:
        void GetOptParam(OutputArray params)const{} //- выдает текущие параметры
        int Iteration(){return 0;} //- делает итерацию
        void SetParticlesNum(int num){} //- число частиц в итерации
        void SetAlpha(double AlphaM){} //- параметр для изменения характера разброса чапстиц при смене уровня
        void GetParamsSTD(OutputArray std)const{}
        void SetParamsSTD(InputArray std){}

        Ptr<Solver::Function> getFunction() const{return Ptr<PFSolver::Function>();}
        void setFunction(const Ptr<Function>& f){}
        TermCriteria getTermCriteria() const{return TermCriteria(0,0,0.0);}
        void setTermCriteria(const TermCriteria& termcrit){}
    private:
        Mat_<double> _std;
        Ptr<Solver::Function> _Function;
        TermCriteria _termcrit;
    };

    Ptr<PFSolver> createPFSolver(const Ptr<Solver::Function>& f,InputArray std,TermCriteria termcrit,int particlesNum,double alpha){
            return Ptr<PFSolverImpl>();
    }
}}
