#include "precomp.hpp"
#include "debug.hpp"
#include <algorithm>

namespace cv{namespace optim{
    class CV_EXPORTS PFSolverImpl : public PFSolver{
    public:
        PFSolverImpl();
        void getOptParam(OutputArray params)const;
        int iteration();
        double minimize(InputOutputArray x);

        void setParticlesNum(int num);
        int getParticlesNum();
        void setAlpha(double AlphaM);
        double getAlpha();
        void getParamsSTD(OutputArray std)const;
        void setParamsSTD(InputArray std);

        Ptr<Solver::Function> getFunction() const;
        void setFunction(const Ptr<Solver::Function>& f);
        TermCriteria getTermCriteria() const;
        void setTermCriteria(const TermCriteria& termcrit);
    private:
        Mat_<double> _std,_particles,weight;
        Ptr<Solver::Function> _Function;
        PFSolver::Function* _real_function;
        TermCriteria _termcrit;
        int _maxItNum,_iter,_particlesNum;
        double _alpha;
    };

    PFSolverImpl::PFSolverImpl(){
        _Function=Ptr<Solver::Function>();
        _real_function=NULL;
        _std=Mat_<double>();
    }
    void PFSolverImpl::getOptParam(OutputArray params)const{
        //TODO: weighted version
        Mat col=_particles.col(std::max_element(weight.begin(),weight.end())-weight.begin());
    }
    int PFSolverImpl::iteration(){
        if(_iter>_maxItNum){
            return _iter;
        }

        //TODO
        //perturb
        //measure
        //normalize
        //replicate
        Mat_<double> new_particles(_std.cols,_particlesNum);
        //...

        if(_particles.cols!=new_particles.cols){
            _particles=new_particles;
        }else{
            new_particles.copyTo(_particles);
        }
        _iter++;
        _std=_std*_alpha;
        return _iter;
    }
    double PFSolverImpl::minimize(InputOutputArray x){
        CV_Assert(_Function.empty()==false);
        CV_Assert(_std.rows==1 && _std.cols>0);
        Mat mat_x=x.getMat();
        CV_Assert(mat_x.type()==CV_64FC1 && MIN(mat_x.rows,mat_x.cols)==1 && MAX(mat_x.rows,mat_x.cols)==_std.cols);

        _iter=0;
        _particles=Mat_<double>(_std.cols,_particlesNum);
        for(int i=0;i<_particles.rows;i++){
            _particles.row(i).setTo(mat_x.begin<double>()[i]);
        }

        //TODO: assign measure
        weight.create(1,_std.cols);
        weight.setTo(0.0);
        return 0.0;
    }

    void PFSolverImpl::setParticlesNum(int num){
        CV_Assert(num>0);
        _particlesNum=num;
    }
    int PFSolverImpl::getParticlesNum(){
        return _particlesNum;
    }
    void PFSolverImpl::setAlpha(double AlphaM){
        CV_Assert(0<AlphaM && AlphaM<=1);
        _alpha=AlphaM;
    }
    double PFSolverImpl::getAlpha(){
        return _alpha;
    }
    Ptr<Solver::Function> PFSolverImpl::getFunction() const{
        return _Function;
    }
    void PFSolverImpl::setFunction(const Ptr<Solver::Function>& f){
        CV_Assert(f.empty()==false);
        Ptr<Solver::Function> myf=f;
        PFSolver::Function *pff=dynamic_cast<PFSolver::Function*>(&(*myf));
        CV_Assert(pff!=NULL);
        _Function=f;
        _real_function=pff;
    }
    TermCriteria PFSolverImpl::getTermCriteria() const{
        return TermCriteria(TermCriteria::MAX_ITER,_maxItNum,0.0);
    }
    void PFSolverImpl::setTermCriteria(const TermCriteria& termcrit){
        CV_Assert(termcrit.type==TermCriteria::MAX_ITER && termcrit.maxCount>0);
        _maxItNum=termcrit.maxCount;
    }
    void PFSolverImpl::getParamsSTD(OutputArray std)const{
        std.create(1,_std.cols,CV_64FC1);
        _std.copyTo(std);
    }
    void PFSolverImpl::setParamsSTD(InputArray std){
        Mat m=std.getMat();
        dprintf(("m.cols=%d\nm.rows=%d\n",m.cols,m.rows));
        CV_Assert(MIN(m.cols,m.rows)==1 && m.type()==CV_64FC1);
        int ndim=MAX(m.cols,m.rows);
        if(ndim!=_std.cols){
            _std=Mat_<double>(1,ndim);
        }
        if(m.rows==1){
            m.copyTo(_std);
        }else{
            Mat std_t=Mat_<double>(ndim,1,(double*)_std.data);
            m.copyTo(std_t);
        }
    }

    Ptr<PFSolver> createPFSolver(const Ptr<Solver::Function>& f,InputArray std,TermCriteria termcrit,int particlesNum,double alpha){
            Ptr<PFSolverImpl> ptr(new PFSolverImpl());

            if(f.empty()==false){
                ptr->setFunction(f);
            }
            Mat mystd=std.getMat();
            if(mystd.cols!=0 || mystd.rows!=0){
                ptr->setParamsSTD(std);
            }
            ptr->setTermCriteria(termcrit);
            ptr->setParticlesNum(particlesNum);
            ptr->setAlpha(alpha);
            return ptr;
    }
}}
