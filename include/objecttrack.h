#ifndef FACETRACKER_H
#define FACETRACKER_H

#include <string>
#include <vector>
#include <opencv2/opencv.hpp>
#include <opencv2/tracking.hpp>

#include "common.h"
using cv::TrackerKCF;

class MultiTrackerKCF
{
public:
	MultiTrackerKCF();
	~MultiTrackerKCF();
	void setROI(const int, const int);
	bool add(const Mat& image, const Rect2d& boundingBox);
	void remove(const int index);
	bool update(const Mat& image);
	vector<Rect2d> objects;

protected:
	vector< Ptr<TrackerKCF> > trackerList;
	int zoom_x = 2;
	int zoom_y = 2;
};


class ObjectTrack
{
public:
    ObjectTrack();
    ~ObjectTrack();
	virtual void setType(const string& type);
    virtual void update(const Mat& img, const vector<vector<int>>& boxes);
	virtual vector<Rect2d> getBoxes();

	int getObjectNum();
	vector<double> getMovements();

    void add_test(const Mat& img, const vector<int>& box);
    void update_test(const Mat& img);
    Rect2d getCurrentBox_test();

protected:
	MultiTrackerKCF* multiTracker;
    vector<Rect2d> objects;		// current tracking objects
    vector<double> movements;	// move distance of each object, starting from firstly detected, only at Y
    vector<int> times;			// frame number of being still for each object
	int maxObjects = 10;

private:
    /*private func*/
    void _addBox(const Mat& img, const vector<int>& new_box);
    void _updateMovements();
    void _updateTimes();
    void _updateObjects();
    void _removeStillBox();
	void _removeNestedBox();
	void _removeOverlapBox();

	int _isOldBox(vector<int> box);

	// parameter
	// for kcf
	int stillImageTime = 3;
	int stillDisThreshold = 10;
	int tracking_zoom_x = 2;
	int tracking_zoom_y = 5;
};


class ObjectTrackVertical : public ObjectTrack
{
public:
	ObjectTrackVertical();
	~ObjectTrackVertical();
	virtual void setType(const string& type);
	virtual void update(const Mat& img, const vector<vector<int>>& boxes);
	virtual vector<Rect2d> getBoxes();

private:
	void _convert(const vector<vector<int>>&, vector<Rect2d>&);
	void _filterOjbects(vector<Rect2d>);
	int _findPair(const Rect2d&, const vector<Rect2d>&);
	bool _isPair(const Rect2d&, const Rect2d&);

	vector<int> life; // together with vector<Rect> objects

	// parameters
	double x_padding = 0.3;
	int lifelength = 5;
};


#endif
