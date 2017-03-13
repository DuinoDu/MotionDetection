#include "../include/objecttrack.h"
#include <algorithm>

#include <iostream>
using std::cout;
using std::endl;

/* MultiTrackerKCF */

MultiTrackerKCF::MultiTrackerKCF(){}
MultiTrackerKCF::~MultiTrackerKCF(){}
void MultiTrackerKCF::setROI(const int _zoom_x, const int _zoom_y)
{
	zoom_x = _zoom_x;
	zoom_y = _zoom_y;
}
bool MultiTrackerKCF::add(const Mat& image, const Rect2d& boundingBox)
{
	Ptr<TrackerKCF> newTracker = TrackerKCF::createTracker();
	newTracker->setROI(zoom_x, zoom_y);
	trackerList.push_back(newTracker);
	objects.push_back(boundingBox); 
	return trackerList.back()->init(image, boundingBox);
}
void MultiTrackerKCF::remove(const int index)
{
	if (trackerList.size() > 0){
		assert(index > 0 && index < trackerList.size());
		assert(trackerList.size() == objects.size());
		trackerList.erase(trackerList.begin() + index);
		objects.erase(objects.begin() + index);
	}
}
bool MultiTrackerKCF::update(const Mat& image)
{
	for (size_t i = 0; i < trackerList.size(); i++){
		trackerList[i]->update(image, objects[i]);
	}
	return true;
}

/* ObjectTrack */

ObjectTrack::ObjectTrack()
{
}

ObjectTrack::~ObjectTrack()
{
    delete multiTracker;
}

void ObjectTrack::setType(const string& type)
{
	assert(type == "KCF");
	CV_UNUSED(type);
    multiTracker = new MultiTrackerKCF();
	multiTracker->setROI(tracking_zoom_x, tracking_zoom_y);
}

void ObjectTrack::update(const Mat& img, const vector<vector<int>>& boxes)
{
    // 1. update old box position---movements, times
    // 2. delete still boxes
    // 3. add new box

	if (objects.size() > maxObjects)
		return;

    if(multiTracker->objects.size() > 0){
        multiTracker->update(img);		// update multiTracker.objects
        _updateMovements();
        _updateTimes();
    }
    _removeStillBox();  // if box.time < 0
	
    for(auto box : boxes){
        _addBox(img, box);
    }
    _updateObjects();
	_removeNestedBox();
	_removeOverlapBox(); // useless ???
}

int ObjectTrack::getObjectNum()
{
    return objects.size();
}

vector<Rect2d> ObjectTrack::getBoxes()
{
    return multiTracker->objects;
}

vector<double> ObjectTrack::getMovements()
{
    return movements;
}

void ObjectTrack::_addBox(const Mat& img, const vector<int>& new_box)
{
    assert(img.isContinuous());
    assert(new_box.size() == 4);
	int index = _isOldBox(new_box);
	if (index == -1){
        Rect2d newObject((double)new_box[0], (double)new_box[1], (double)(new_box[2]-new_box[0]), (double)(new_box[3]-new_box[1]));
        objects.push_back(newObject);
        movements.push_back(0);
        times.push_back(stillImageTime);
        multiTracker->add(img, newObject);
    }
	else{
		objects[index].x = (double)new_box[0];
		objects[index].y = (double)new_box[1];
		objects[index].width = (double)(new_box[2] - new_box[0]);
		objects[index].height = (double)(new_box[3] - new_box[1]);
	}
}

void ObjectTrack::_updateMovements()
{
    assert(objects.size() == multiTracker->objects.size());
    assert(objects.size() == movements.size());
    for(int i = 0; i < movements.size(); ++i){
        double old_y = objects[i].y + objects[i].height/2;
        double new_y = multiTracker->objects[i].y + multiTracker->objects[i].height/2;
        movements[i] += new_y - old_y;
    }
}

void ObjectTrack::_updateTimes()
{
    assert(objects.size() == multiTracker->objects.size());
    assert(objects.size() == times.size());
    for(int i = 0; i < times.size(); ++i){
        double old_x = objects[i].x + objects[i].width / 2;
		double old_y = objects[i].y + objects[i].height / 2;
        double new_x = multiTracker->objects[i].x + multiTracker->objects[i].width/2;
        double new_y = multiTracker->objects[i].y + multiTracker->objects[i].height/2;
        double distance = std::abs(old_x - new_x) + std::abs(old_y - new_y);
        //cout << "distance: " << distance << endl;
        if(distance < stillDisThreshold){
            times[i]--;
        }
        else{
            times[i] = stillImageTime;
        }
    }
}

void ObjectTrack::_updateObjects()
{
    objects = multiTracker->objects;
}

void ObjectTrack::_removeStillBox()
{
    assert(objects.size() == multiTracker->objects.size());
    assert(objects.size() == movements.size());
    assert(objects.size() == times.size());
    for(int i = 0; i < objects.size(); ++i){
		// remove still box
        if(times[i] < 0){
            //multiTracker->objects.erase(multiTracker->objects.begin() + i);
            multiTracker->remove(i);  // custom function in opencv
            objects.erase(objects.begin() + i);
            movements.erase(movements.begin() + i);
            times.erase(times.begin() + i);
        }
    }
}

void ObjectTrack::_removeNestedBox()
{
	assert(objects.size() == multiTracker->objects.size());
	assert(objects.size() == movements.size());
	assert(objects.size() == times.size());
	for (int i = 0; i < objects.size(); ++i){
		// remove nested box
		Point2d center(objects[i].x + objects[i].width / 2, objects[i].y + objects[i].height / 2);
		bool flag = false;
		for (auto object : objects){
			if (center.x == objects[i].x + objects[i].width / 2 && center.y == objects[i].y + objects[i].height / 2)
				continue;
			
			if (object.contains(center)){
				flag = true;
				break;
			}
		}
		if (flag){
			multiTracker->remove(i);  // custom function in opencv
			objects.erase(objects.begin() + i);
			movements.erase(movements.begin() + i);
			times.erase(times.begin() + i);
		}
	}
}

void ObjectTrack::_removeOverlapBox()
{
	assert(objects.size() == multiTracker->objects.size());
	assert(objects.size() == movements.size());
	assert(objects.size() == times.size());

	for (int i = 0; i < objects.size(); ++i){
		// remove overlap box
		Point2d center(objects[i].x + objects[i].width / 2, objects[i].y + objects[i].height / 2);
		bool flag = false;
		for (auto object : objects){
			if (center.x == objects[i].x + objects[i].width / 2 && center.y == objects[i].y + objects[i].height / 2)
				continue;		

			cout << "computing overlap" << endl;
			int x1 = max(objects[i].x, object.x);
			int y1 = max(objects[i].y, object.y);
			int x2 = min(objects[i].x + objects[i].width, object.x + object.width);
			int y2 = min(objects[i].y + objects[i].height, object.y + object.height);
			int h = max(0, x2 - x1 + 1);
			int w = max(0, y2 - y1 + 1);
			double inter = h * w;
			double overlap = inter / (objects[i].width * objects[i].height + object.width * object.height - inter);
			cout << "overlap:" << overlap << endl;
			if (overlap > 0.2){
				assert(false);
				flag = true;
				break;
			}
		}
		if (flag){
			multiTracker->remove(i);  // custom function in opencv
			objects.erase(objects.begin() + i);
			movements.erase(movements.begin() + i);
			times.erase(times.begin() + i);
		}
	}
}

int ObjectTrack::_isOldBox(vector<int> box)
{
	int index = -1;
    Point2d center((box[0] + box[2])/2, (box[1] + box[3])/2);

	/*
	Point2d p1(box[0], box[1]);
	Point2d p2(box[0], box[3]);
	Point2d p3(box[2], box[1]);
	Point2d p4(box[2], box[3]);
    for(auto object : objects){
        if(object.contains(p1) ||
           object.contains(p2) ||
           object.contains(p3) ||
           object.contains(p4) ||
           object.contains(center)){
			return index;
        }
    }
	*/
	for (size_t i = 0; i < objects.size(); ++i){
		if (objects[i].x < center.x && center.x < objects[i].x + objects[i].width){
			index = i;
			break;
		}
	}
	return index;
}

void ObjectTrack::add_test(const Mat& img, const vector<int>& box)
{
    _addBox(img, box);
}
void ObjectTrack::update_test(const Mat& img)
{
    multiTracker->update(img);
}
Rect2d ObjectTrack::getCurrentBox_test()
{
    return multiTracker->objects.back();
}




/* ObjectTrackVertical */

ObjectTrackVertical::ObjectTrackVertical(){}
ObjectTrackVertical::~ObjectTrackVertical(){}
void ObjectTrackVertical::setType(const string& type){ CV_UNUSED(type); }
void ObjectTrackVertical::update(const Mat& img, const vector<vector<int>>& boxes)
{
	vector<Rect2d> boxesRect;
	_convert(boxes, boxesRect);
	_filterOjbects(boxesRect);


	assert(objects.size() == movements.size());
	assert(objects.size() == life.size());
	for (size_t i = 0; i < objects.size(); i++){
		// 1. update moving objects
		int index = -1;
		index = _findPair(objects[i], boxesRect);
		assert(index >= -1 && index < (int)boxesRect.size());
		if (index > -1){
			movements[i] += (boxesRect[index].y + boxesRect[index].height / 2) - (objects[i].y + objects[i].height / 2);
			objects[i].x = boxesRect[index].x;
			objects[i].y = boxesRect[index].y;
			objects[i].width = boxesRect[index].width;
			objects[i].height = boxesRect[index].height;
			life[i] = lifelength;
		}

		// 2. remove still objects
		life[i] = (life[i] > 0) ? life[i]-1 : 0 ;
		if (life[i] == 0){
			movements.erase(movements.begin() + i);
			objects.erase(objects.begin() + i);
			life.erase(life.begin() + i);
		}
	}

	// 3. add new object
	for (size_t i = 0; i < boxesRect.size(); i++){
		if (objects.size() > maxObjects)
			break;
		int index_new = -1;
		index_new = _findPair(boxesRect[i], objects);
		//cout << "index when add:" << index_new << ", i:" << i << endl;
		assert(index_new >= -1);
		if (index_new == -1){
			objects.push_back(boxesRect[i]);
			movements.push_back(0);
			life.push_back(lifelength); 
		}
		//cout << "add" << endl;
	}
}

vector<Rect2d> ObjectTrackVertical::getBoxes(){ return objects; }

void ObjectTrackVertical::_convert(const vector<vector<int>>& _boxes, vector<Rect2d>& _objects)
{
	for (auto box : _boxes){
		Rect2d newObject((double)box[0], (double)box[1], (double)(box[2] - box[0]), (double)(box[3] - box[1]));
		_objects.push_back(newObject);
	}
}
void ObjectTrackVertical::_filterOjbects(vector<Rect2d> _objects)
{
	vector<size_t> repeat_index;
	for (size_t i = 0; i < _objects.size(); i++){
		for (size_t j = i + 1; j < _objects.size(); j++){
			if (_isPair(_objects[i], _objects[j]) &&
				find(repeat_index.begin(), repeat_index.end(), i) == repeat_index.end())
			{
				repeat_index.push_back(j);
			}
		}
	}
	for (auto i : repeat_index)
		_objects.erase(_objects.begin() + i);
}
int ObjectTrackVertical::_findPair(const Rect2d& box, const vector<Rect2d>& boxes)
{
	int index = -1;
	for (size_t i = 0; i < boxes.size(); i++){
		if (_isPair(box, boxes[i])){
			index = i;
			break;
		}
	}
	return index;
}
bool ObjectTrackVertical::_isPair(const Rect2d& rect1, const Rect2d& rect2)
{
	double x1 = rect1.x - x_padding * rect1.width;
	double x2 = rect1.x + rect1.width + x_padding * rect1.width;
	return ((rect2.x + rect2.width / 2) >= rect1.x && (rect2.x + rect2.width / 2) <= rect1.x + rect1.width);
}

