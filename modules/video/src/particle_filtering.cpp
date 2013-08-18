#include "precomp.hpp"
#include "opencv2/optim.hpp"
#include "TrackingFunctionPF.hpp"

namespace cv{
    class TrackerTargetStatePF : public TrackerTargetState{
    public:
        Rect getRect(){return rect;}
        TrackerTargetStatePF(const Mat_<double>& row){
            CV_Assert(row.rows==1 && row.cols==4);
            rect=Rect(Point_<int>((int)row(0,0),(int)row(0,1)),Point_<int>((int)row(0,2),(int)row(0,3)));
        }
    protected:
        Rect rect;
    };

    class TrackerModelPF : public TrackerModel{
    public:
        TrackerModelPF( const TrackerPF::Params &parameters,const Mat& image,const Rect& boundingBox );
    protected:
        void modelEstimationImpl( const std::vector<Mat>& responses );
        void modelUpdateImpl();
    private:
        Ptr<optim::PFSolver> _solver;
        Ptr<TrackingFunctionPF> _function;
        Mat_<double> _std,_last_guess;
    };

    void TrackerPF::Params::read(const FileNode& ){
    }
    void TrackerPF::Params::write( FileStorage& ) const{
    }
    void TrackerModelPF::modelUpdateImpl(){
    }

    TrackerModelPF::TrackerModelPF( const TrackerPF::Params &parameters,const Mat& image,const Rect& boundingBox ){
        _function=new TrackingFunctionPF(image(boundingBox));
        _solver=createPFSolver(_function,parameters.std,TermCriteria(TermCriteria::MAX_ITER,parameters.iterationNum,0.0),
                    parameters.particlesNum,parameters.alpha);
        _std=parameters.std;
        _last_guess=(Mat_<double>(1,4)<<(double)boundingBox.x,(double)boundingBox.y,
                (double)boundingBox.x+boundingBox.width,(double)boundingBox.y+boundingBox.height);
    }
    void TrackerModelPF::modelEstimationImpl( const std::vector<Mat>& responses ){
        CV_Assert(responses.size()==1);
        Mat image=responses[0];
        Ptr<TrackerTargetState> ptr;
        if(false){
            //real implementation
            //TODO - here we do iterations
            _solver->setParamsSTD(_std);
            _solver->minimum(_last_guess);
        }else{
            //Mat_<double> row=(Mat_<double>(1,4)<<0.0,0.0,(double)image.cols/2,(double)image.rows/2);
            Mat_<double> row=_last_guess;
            ptr=new TrackerTargetStatePF(row);
        }

        setLastTargetState(ptr);
    }
    TrackerPF::Params::Params(){
        //if these def params will be changed, it might also have sense to change def params at optim.hpp
        iterationNum=5;
        particlesNum=100;
        alpha=0.6;
        std=(Mat_<double>(1,4)<<1.0,1.0,1.0,1.0); 
    }
    TrackerPF::TrackerPF( const TrackerPF::Params &parameters){
        params=parameters;
        initialized=false;
        featureSet=0;
        sampler=0;
        model=0;
    }
    bool TrackerPF::initImpl( const Mat& image, const Rect& boundingBox ){
        model=Ptr<TrackerModel>(new TrackerModelPF(params,image,boundingBox));
        initialized=true;
        return true;
    }
    bool TrackerPF::updateImpl( const Mat& image, Rect& boundingBox ){
        model->modelEstimation(std::vector<Mat>(1,image));
        TrackerTargetStatePF* state=dynamic_cast<TrackerTargetStatePF*>(static_cast<TrackerTargetState*>(model->getLastTargetState()));
        boundingBox=state->getRect();
        return true;
    }
}

/*class TrackerModel
{
 public:
  TrackerModel();
  virtual ~TrackerModel(){}
  bool setTrackerStateEstimator( Ptr<TrackerStateEstimator> trackerStateEstimator ){}
  void modelUpdate(){}
  bool runStateEstimator(){}
  Ptr<TrackerStateEstimator> getTrackerStateEstimator() const{}

  void setLastTargetState( const Ptr<TrackerTargetState>& lastTargetState );
  Ptr<TrackerTargetState> getLastTargetState() const;
  void modelEstimation( const std::vector<Mat>& responses );
  const std::vector<ConfidenceMap>& getConfidenceMaps() const;
  const ConfidenceMap& getLastConfidenceMap() const;
 protected:
  std::vector<ConfidenceMap> confidenceMaps;
  Ptr<TrackerStateEstimator> stateEstimator;
  ConfidenceMap currentConfidenceMap;
  Trajectory trajectory;
  virtual void modelEstimationImpl( const std::vector<Mat>& responses ) - make an iteration here
  virtual void modelUpdateImpl(){}
};
    typedef std::vector<std::pair<Ptr<TrackerTargetState>, float> > ConfidenceMap;
    typedef std::vector<Ptr<TrackerTargetState> > Trajectory;
*/
