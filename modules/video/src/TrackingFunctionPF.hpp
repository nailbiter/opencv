#include <cmath>
#include <opencv2/imgproc/imgproc_c.h>
#define CLIP(x,a,b) MIN(MAX((x),(a)),(b))
namespace cv{
    class TrackingFunctionPF : public optim::PFSolver::Function{
        public:
            TrackingFunctionPF(const Mat& chosenRect);
            void update(const Mat& image);
            double calc(const double* x) const;
            void correctParams(double* pt)const;
        private:
            Mat _image;
            static inline Rect rectFromRow(const double* row);
            const int _nh,_ns,_nv;
            class TrackingHistogram{
            public:
                TrackingHistogram(const Mat& img,int nh,int ns,int nv);
                double dist(const TrackingHistogram& hist)const;
            private:
                Mat_<double> HShist, Vhist;
            };
            TrackingHistogram _origHist;
    };

    TrackingFunctionPF::TrackingHistogram::TrackingHistogram(const Mat& img,int nh,int ns,int nv){
        printf("histogram constructor was called with %d, %d and %d\n",nh,ns,nv);

        uchar min=255,max=0;
        for(int i=0;i<img.rows;i++){
            for(int j=0;j<img.cols;j++){
                Vec3b v=img.at<Vec3b>(i,j);
                max=MAX(max,v.val[0]);
                max=MAX(max,v.val[1]);
                max=MAX(max,v.val[2]);
                min=MIN(min,v.val[0]);
                min=MIN(min,v.val[1]);
                min=MIN(min,v.val[2]);
            }}

        Mat hsv(img.rows,img.cols,CV_MAKETYPE(CV_32F,3));
        cvtColor(img,hsv,CV_BGR2HSV);
        HShist=Mat_<double>(nh,ns,0.0);
        Vhist=Mat_<double>(1,nv,0.0);

        printf("hi from line %d\n",__LINE__);
        for(int i=0;i<img.rows;i++){
            for(int j=0;j<img.cols;j++){
                Vec3f pt=hsv.at<Vec3f>(i,j);

                Vec3b v=img.at<Vec3b>(i,j);
                printf("dimensions: %dx%d, min--max=%d--%d\n",img.rows,img.cols,(int)min,(int)max);
                printf("%d %d\n",i,j);
                printf(" (%d %d %d)->(%f %f %f)\n",(int)v[0],(int)v[1],(int)v[2],pt.val[0],pt.val[1],pt.val[2]);
                fflush(stdout);

                if(pt.val[1]>0.1 && pt.val[2]>0.2){
                    HShist(MIN(nh-1,(int)(nh*pt.val[0]/360.0)),MIN(ns-1,(int)(ns*pt.val[1])))++;
                }else{
                    Vhist(0,MIN(nv-1,(int)(nv*pt.val[2])))++;
                }
            }}

        printf("hi from line %d\n",__LINE__);
        double total=*(sum(HShist)+sum(Vhist)).val;
        HShist/=total;
        Vhist/=total;
    }
    double TrackingFunctionPF::TrackingHistogram::dist(const TrackingHistogram& hist)const{
        double res=1.0;

        for(int i=0;i<HShist.rows;i++){
            for(int j=0;j<HShist.cols;i++){
                res-=sqrt(HShist(i,j)*hist.HShist(i,j));
            }}
        for(int j=0;j<Vhist.cols;j++){
            res-=sqrt(Vhist(0,j)*hist.Vhist(0,j));
        }

        return sqrt(res);
    }
    double TrackingFunctionPF::calc(const double* x) const{
        return _origHist.dist(TrackingHistogram(_image(rectFromRow(x)),_nh,_ns,_nv));
    }
    TrackingFunctionPF::TrackingFunctionPF(const Mat& chosenRect):_nh(256),_ns(256),_nv(256),_origHist(chosenRect,_nh,_ns,_nv){
        printf("function constructor was called\n");
    }
    void TrackingFunctionPF::update(const Mat& image){
        _image=image;
    }
    void TrackingFunctionPF::correctParams(double* pt)const{
        pt[0]=CLIP(pt[0],0.0,_image.cols+0.9);
        pt[1]=CLIP(pt[1],0.0,_image.rows+0.9);
        pt[2]=CLIP(pt[2],0.0,_image.cols+0.9);
        pt[3]=CLIP(pt[3],0.0,_image.rows+0.9);
    }
    Rect TrackingFunctionPF::rectFromRow(const double* row){
        return Rect(Point_<int>((int)row[0],(int)row[1]),Point_<int>((int)row[2],(int)row[3]));
    }
}
