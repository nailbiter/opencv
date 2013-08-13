#include "precomp.hpp"
#define ALEX_DEBUG
#include "debug.hpp"
#include "opencv2/core/core_c.h"
#include <algorithm>
#include <typeinfo>
#include <cmath>

namespace cv{namespace optim{
    class PFSolverImpl : public PFSolver{
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
        Mat_<double> _std,_particles,_logweight;//TODO: particles - transpose
        Ptr<Solver::Function> _Function;
        PFSolver::Function* _real_function;
        TermCriteria _termcrit;
        int _maxItNum,_iter,_particlesNum;
        double _alpha;
        inline void normalize(Mat_<double>& row);
        RNG rng;
    };

    PFSolverImpl::PFSolverImpl(){
        _Function=Ptr<Solver::Function>();
        _real_function=NULL;
        _std=Mat_<double>();
        rng=RNG(getTickCount());
    }
    void PFSolverImpl::getOptParam(OutputArray params)const{
        params.create(1,_std.rows,CV_64FC1);
#ifdef WEIGHTED
        params.setTo(0.0);
        for(int i=0;i<_particles.rows;i++){
            params+=_particles.row(i)/exp(-_logweight(0,i));
        }
#else
        params.create(1,_std.rows,CV_64FC1);
        _particles.row(std::max_element(_logweight.begin(),_logweight.end())-_logweight.begin()).copyTo(params);
#endif
    }
    int PFSolverImpl::iteration(){
        if(_iter>=_maxItNum){
            return _maxItNum+1;
        }

        _real_function->setLevel(_iter+1,_maxItNum);

        //perturb
        for(int j=0;j<_particles.cols;j++){
            double sigma=_std(0,j);
            for(int i=0;i<_particles.rows;i++){
                    _particles(i,j)+=rng.gaussian(sigma);
            }
        }

        //measure
        for(int i=0;i<_particles.rows;i++){
            _real_function->correctParams((double*)_particles.row(i).data);
            _logweight(0,i)=-(_real_function->calc((double*)_particles.row(i).data));
        }
        //normalize
        normalize(_logweight);
        //replicate
        Mat_<double> new_particles(_particlesNum,_std.cols);
        int num_particles=0;
        for(int i=0;i<_particles.rows;i++){
            int num_replicons=cvFloor(new_particles.rows/exp(-_logweight(0,i)));
            for(int j=0;j<num_replicons;j++,num_particles++){
                _particles.row(i).copyTo(new_particles.row(num_particles));
            }
        }
        Mat_<double> maxrow=_particles.row(std::max_element(_logweight.begin(),_logweight.end())-_logweight.begin());
        for(;num_particles<new_particles.rows;num_particles++){
                maxrow.copyTo(new_particles.row(num_particles));
        }

        if(_particles.rows!=new_particles.rows){
            _particles=new_particles;
        }else{
            new_particles.copyTo(_particles);
        }
        _std=_std*_alpha;
        _iter++;
        return _iter;
    }
    double PFSolverImpl::minimize(InputOutputArray x){
        CV_Assert(_Function.empty()==false);
        CV_Assert(_std.rows==1 && _std.cols>0);
        Mat mat_x=x.getMat();
        CV_Assert(mat_x.type()==CV_64FC1 && MIN(mat_x.rows,mat_x.cols)==1 && MAX(mat_x.rows,mat_x.cols)==_std.cols);

        _iter=0;
        _particles=Mat_<double>(_particlesNum,_std.cols);
        if(mat_x.rows>1){
            mat_x=mat_x.t();
        }
        for(int i=0;i<_particles.rows;i++){
            mat_x.copyTo(_particles.row(i));
        }

        _logweight.create(1,_particles.rows);
        _logweight.setTo(-log(_particles.rows));
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

        Ptr<Solver::Function> non_const_f(f);
        Solver::Function* f_ptr=static_cast<Solver::Function*>(non_const_f);

        dprintf(("here is a pointer in setter: %p\n",f_ptr));
        dprintf(("name: %s\n",typeid(*f_ptr).name()));

        PFSolver::Function *pff=dynamic_cast<PFSolver::Function*>(f_ptr);
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
    void PFSolverImpl::normalize(Mat_<double>& row){
        double logsum=0.0;
        double max=*(std::max_element(row.begin(),row.end()));
        row-=max;
        for(int i=0;i<row.cols;i++){
            logsum+=exp(row(0,i));
        }
        logsum=log(logsum);
        row-=logsum;
    }
}}
