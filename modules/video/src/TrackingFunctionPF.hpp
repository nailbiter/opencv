namespace cv{
    class TrackingFunctionPF : public optim::PFSolver::Function{
        public:
            TrackingFunctionPF(const Mat& chosenRect){}
            void update(const Mat& image){}
            double calc(const double* x) const{return 0.0;}
            void correctParams(double* )const{}
    };
}
