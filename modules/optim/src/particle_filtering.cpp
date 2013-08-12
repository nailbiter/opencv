#include "precomp.hpp"
#include "debug.hpp"

namespace cv{namespace optim{
    class CV_EXPORTS PFSolverImpl : public PFSolver{
    public:
        PFSolverImpl();
        void getOptParam(OutputArray params)const {} //- выдает текущие параметры
        int iteration() {return 0;} //- делает итерацию
        void setParticlesNum(int num){} //- число частиц в итерации
        void setAlpha(double AlphaM){} //- параметр для изменения характера разброса чапстиц при смене уровня
        void getParamsSTD(OutputArray std)const {}
        void setParamsSTD(InputArray std){}

        Ptr<Solver::Function> getFunction() const;
        void setFunction(const Ptr<Function>& f);
        TermCriteria getTermCriteria() const;
        void setTermCriteria(const TermCriteria& termcrit);
    private:
        Mat_<double> _std;
        Ptr<Solver::Function> _Function;
        TermCriteria _termcrit;
        int _maxItNum,_iter;
        double _alpha;
    };

    PFSolverImpl::PFSolverImpl(){
    }
    Ptr<Solver::Function> getFunction() const{
        return _Function;
    }
    void PFSolverImpl::setFunction(const Ptr<Function>& f){
        CV_Assert(f.empty()==false);
        _Function=f;
    }
    TermCriteria PFSolverImpl::getTermCriteria() const{
        return TermCriteria(TermCriteria::MAX_ITER,_maxItNum,0.0);
    }
    void PFSolverImpl::setTermCriteria(const TermCriteria& termcrit){
        CV_Assert(termcrit.type==TermCriteria::MAX_ITER && termcrit.maxCount>0);
        _maxItNum=termcrit.maxCount;
    }

    Ptr<PFSolver> createPFSolver(const Ptr<Solver::Function>& f,InputArray std,TermCriteria termcrit,int particlesNum,double alpha){
            return Ptr<PFSolverImpl>();
    }
}}
