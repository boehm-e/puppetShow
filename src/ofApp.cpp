#include "ofApp.h"


void ofApp::setup()
{
    int ratio = 2;
    camWidth = 640 / ratio;  // try to grab at this size.
    camHeight = 480 / ratio;

    vidGrabber.setDeviceID(0);
    vidGrabber.setDesiredFrameRate(30);
    vidGrabber.initGrabber(camWidth, camHeight);

    detector = dlib::get_frontal_face_detector();
    dlib::deserialize(ofToDataPath("shape_predictor_68_face_landmarks.dat", true)) >> sp;
}

void ofApp::update() {
    vidGrabber.update();
}



void ofApp::processFace() {
    ofPixels pix = vidGrabber.getPixels();

    // Make the image larger so we can detect small faces.
    dlib::pyramid_up(pix);

    // Now tell the face detector to give us a list of bounding boxes
    // around all the faces in the image.
    std::vector<dlib::rectangle> dets = detector(pix);

    if (dets.size() == 1)
    for (std::size_t j = 0; j < dets.size(); ++j)
    {
        dlib::full_object_detection shape = sp(pix, dets[j]);
        shapes.push_back(shape);
    }
    image.setFromPixels(pix);
}



int ofApp::getLeftEyebrowOpen(dlib::full_object_detection shape) {
    int leftEyebrow = ofMap(shape.part(37).y() - shape.part(19).y(), MIN_EYEBROW_OPEN, MAX_EYEBROW_OPEN, 0, 100);
    return leftEyebrow;
};

int ofApp::getRightEyebrowOpen(dlib::full_object_detection shape) {
    int rightEyebrow = ofMap(shape.part(44).y() - shape.part(24).y(), MIN_EYEBROW_OPEN, MAX_EYEBROW_OPEN, 0, 100);
    return rightEyebrow;
};

int ofApp::getMouthOpen(dlib::full_object_detection shape) {
    int minMouthOpen = 0;
    int maxMouthOpen = 60;
    int mouthOpen = ofMap(shape.part(67).y() - shape.part(63).y(), MIN_MOUTH_OPEN, MAX_MOUTH_OPEN, 0, 100);
    return mouthOpen;
};


void ofApp::draw()
{
    shapes.clear();
    processFace();

    image.draw(0, 0);
    for (auto& shape: shapes)
    {
        ofSetColor(ofColor::red);
        for (std::size_t i = 0; i < shape.num_parts(); ++i) {
            ofDrawCircle(shape.part(i).x(), shape.part(i).y(), 2);
//            ofDrawCircle(ofxDlib::toOf(shape.part(i)), 2);
        }
        ofSetColor(ofColor::white);
    }


    for (auto& shape: shapes) {
        int mouthOpen = getMouthOpen(shape);
        int leftEyebrow = getLeftEyebrowOpen(shape);
        int rightEyebrow = getRightEyebrowOpen(shape);

        ofLogNotice() << "MOUTH OPEN : " << mouthOpen << "%";
        ofLogNotice() << "LFT EB OPEN : " << leftEyebrow << "%";
        ofLogNotice() << "RGT EB OPEN : " << rightEyebrow << "%";
    }

    ofPopMatrix();

}


