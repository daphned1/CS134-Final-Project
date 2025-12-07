//--------------------------------------------------------------
//
//  Kevin M. Smith
//
//  Octree Test - startup scene
// 
//
//  Student Name:   < Your Name goes Here >
//  Date: <date of last version>


#include "ofApp.h"
#include "Util.h"


//--------------------------------------------------------------
// setup scene, lighting, state and load geometry
//
void ofApp::setup(){
	bWireframe = false;
	bDisplayPoints = false;
	bAltKeyDown = false;
	bCtrlKeyDown = false;
	bLanderLoaded = false;
	bTerrainSelected = true;

	ofSetWindowShape(1024, 768);
	ofSetFrameRate(60);

	ofEnableSmoothing();
	ofEnableDepthTest();

	/* -------------------------------------------------------------------------- */
	/*                           Load Texture                                     */
	/* -------------------------------------------------------------------------- */

	// texture loading

	ofDisableArbTex();     // disable rectangular textures

	// load textures

	if (!ofLoadImage(particleTex, "images/dot.png")) {
		cout << "Particle Texture File: images/dot.png not found" << endl;
		ofExit();
	}

	/* -------------------------------------------------------------------------- */
	/*                           Load Shader                                      */
	/* -------------------------------------------------------------------------- */
#ifdef TARGET_OPENGLES
	shader.load("shaders_gles/shader");
#else
	shader.load("shaders/shader");
#endif

	/* -------------------------------------------------------------------------- */
	/*                           Load Background Image                            */
	/* -------------------------------------------------------------------------- */
	backgroundImg.load("images/galaxy_bkgrd.png");

	/* -------------------------------------------------------------------------- */
	/*                           Load Sounds                                      */
	/* -------------------------------------------------------------------------- */
	backgroundSnd.load("sounds/spaceBackgroundSnd.mp3");
	backgroundSnd.setLoop(true);
	backgroundSnd.setVolume(0.2f);

	explosionSnd.load("sounds/explosionSnd.mp3");
	explosionSnd.setVolume(1.0f);

	thrusterSnd.load("sounds/thrustSnd.mp3");
	thrusterSnd.setLoop(true);
	thrusterSnd.setVolume(1.0f);

	forwardSnd.load("sounds/forwardSnd.mp3");
	forwardSnd.setVolume(1.0f);

	backwardSnd.load("sounds/backwardSnd.mp3");
	backwardSnd.setVolume(1.0f);

	countdownSnd.load("sounds/countdownSnd.mp3");
	countdownSnd.setVolume(1.0f);

	successLandSnd.load("sounds/successLand.mp3");
	successLandSnd.setVolume(1.0f);

	gameOverSnd.load("sounds/lostLifeSnd.mp3");
	gameOverSnd.setLoop(true);

	//turn on background sound
	backgroundSnd.play();


	/* -------------------------------------------------------------------------- */
	/*                                 Load models                                */
	/* -------------------------------------------------------------------------- */
	mars.loadModel("geo/BigMoonBaseBrighter.obj");
	mars.setScaleNormalization(false);
	

	if (lander.loadModel("geo/MoonLander.obj")) {
		bLanderLoaded = true;
		lander.setScaleNormalization(false);
		lander.setPosition(0, 50, 0);
		landerPos = glm::vec3(0, 50, 0);
		// cout << "number of meshes: " << lander.getNumMeshes() << endl;
		bboxList.clear();
		for (int i = 0; i < lander.getMeshCount(); i++) {
			bboxList.push_back(Octree::meshBounds(lander.getMesh(i)));
		}
	}

	gforce = new GravityForce(ofVec3f(0, -2, 0));
	tforce = new TurbulenceForce(ofVec3f(-10, -10, -10), ofVec3f(10, 10, 10));
	radialForce = new ImpulseRadialForce(10);

	/* -------------------------------------------------------------------------- */
	/*                                  Emitters                                  */
	/* -------------------------------------------------------------------------- */
	// Lander Emitter
	emitter.sys->addForce(tforce);
	emitter.sys->addForce(gforce);
	emitter.sys->addForce(radialForce);

	emitter.setVelocity(ofVec3f(0, -10, 0));
	emitter.setEmitterType(RadialEmitter);
	emitter.setGroupSize(20);
	emitter.setRate(50);
	emitter.setRandomLife(true);
	emitter.setLifespanRange(ofVec2f(0, 0.5));
	emitter.setParticleRadius(10);

	// Explosion Emitter
	explosionEmitter.sys->addForce(tforce);
	explosionEmitter.sys->addForce(gforce);
	explosionEmitter.sys->addForce(radialForce);

	explosionEmitter.setVelocity(ofVec3f(0, 30, 0));
	explosionEmitter.setOneShot(true);
	explosionEmitter.setEmitterType(RadialEmitter);
	explosionEmitter.setGroupSize(500);
	explosionEmitter.setRandomLife(true);
	explosionEmitter.setLifespanRange(ofVec2f(2, 4));
	explosionEmitter.setParticleRadius(20);
	explosionEmitter.setMass(1);

	/* -------------------------------------------------------------------------- */
	/*                                 Light                                      */
	/* -------------------------------------------------------------------------- */

	ofSetGlobalAmbientColor(ofColor(100, 100, 100)); //white ambient light

	keyLight.setup();
	keyLight.setDirectional();
	keyLight.setOrientation(glm::vec3(45, -40, 0));
	keyLight.setAmbientColor(ofFloatColor(1.0, 1.0, 1.0));
	keyLight.setDiffuseColor(ofFloatColor(1, 1, 1));
	keyLight.setSpecularColor(ofFloatColor(1, 1, 1));

	keyLight.enable();

	fillLight.setup();
	fillLight.setDirectional();
	fillLight.setOrientation(glm::vec3(-70, 0, 0));
	fillLight.setDiffuseColor(ofFloatColor(0.4, 0.5, 0.8));
	fillLight.setSpecularColor(ofFloatColor(0.2, 0.25, 0.4));
	fillLight.setAmbientColor(ofFloatColor(0.2, 0.24, 0.3));

	fillLight.enable();

	rimLight.setup();
	rimLight.setDirectional();

	rimLight.setOrientation(glm::vec3(20, 160, 0));
	rimLight.setDiffuseColor(ofFloatColor(0.4, 0.4, 0.45));
	rimLight.setSpecularColor(ofFloatColor(0.7, 0.7, 0.8));
	rimLight.setAmbientColor(ofFloatColor(0.05, 0.05, 0.08));

	rimLight.enable();

	landerLight.setup();
	landerLight.setSpotlight();
	landerLight.setSpotlightCutOff(60); // cone angle (in degrees)
	landerLight.setSpotConcentration(30);
	landerLight.setScale(5.0);
	landerLight.setDiffuseColor(ofFloatColor(1.0, 0.95, 0.850));
	landerLight.setSpecularColor(ofFloatColor(1.0, 0.95, 0.85));

	
	
	/* -------------------------------------------------------------------------- */
	/*                                 Cameras                                    */
	/* -------------------------------------------------------------------------- */

	//Main camera
	mainCam.setDistance(10);
	mainCam.setNearClip(.1);
	mainCam.setFov(65.5); // approx equivalent to 28mm in 35mm format
	ofSetVerticalSync(true);
	mainCam.disableMouseInput();

	glm::vec3 landerPos = lander.getPosition();
	mainCam.setPosition(150, 78, 124);
	mainCam.lookAt(landerPos);
	

	//Tracking camera
	trackCam.setPosition(landerPos.x, landerPos.y + 20 , landerPos.z + 45);
	trackCam.lookAt(landerPos);

	//Lander Camera
	landerCam.setPosition(landerPos.x, landerPos.y + 50 , landerPos.z);
	landerCam.lookAt(glm::vec3(0,0,0));

	// Cabin Camera
	cabinCam.setPosition(landerPos.x, landerPos.y + 10, landerPos.z);
	cabinCam.lookAt(glm::vec3(0, 0, 0));

	//Set activeCam = mainCam
	activeCam = &mainCam;

	// setup rudimentary lighting 
	//initLightingAndMaterials();

	/* -------------------------------------------------------------------------- */
	/*                                 GUI                                        */
	/* -------------------------------------------------------------------------- */
	gui.setup();
	gui.add(numLevels.setup("Number of Octree Levels", 1, 1, 10));
	bHide = false;
	gui.add(landerLightOn.set("Lander Light On:", true));
	gui.add(backgroundMusicOn.set("Background Music On:", true));
	camName.set("Camera", "Fixed Camera (1)");
	gui.add(camName);


	/* -------------------------------------------------------------------------- */
	/*                             octree for Testing                             */
	/* -------------------------------------------------------------------------- */
	ofMesh full;
	for (int i = 0; i < mars.getMeshCount(); i++) {
		full.append(mars.getMesh(i));
	}
	octree.create(full, 20);
	
	cout << "Number of Verts: " << mars.getMesh(0).getNumVertices() << endl;

	testBox = Box(Vector3(3, 3, 0), Vector3(5, 5, 2));

}
 
//--------------------------------------------------------------
// incrementally update scene (animation)
//--------------------------------------------------------------

void ofApp::update() {
	/* -------------------------------------------------------------------------- */
	/*                                   Cameras                                  */
	/* -------------------------------------------------------------------------- */
	if (currentCam == 2) {
		ofVec3f currentPos = trackCam.getPosition();
		ofVec3f targetPos = ofVec3f(landerPos.x, landerPos.y + 20, landerPos.z + 45);
		trackCam.setPosition(currentPos.interpolate(targetPos, 0.1));
		trackCam.lookAt(lander.getPosition());
	}
	else if (currentCam == 3) {
		ofVec3f currentPos = landerCam.getPosition();
		ofVec3f targetPos = ofVec3f(landerPos.x, landerPos.y + 50, landerPos.z);
		landerCam.setPosition(currentPos.interpolate(targetPos, 0.1));
		// ONBOARD CAM — attach to lander
		landerCam.lookAt(lander.getPosition(), glm::vec3(0, 0, -1));
	}
	else if (currentCam == 4) {
		ofVec3f currentPos = cabinCam.getPosition();
		ofVec3f targetPos = ofVec3f(landerPos.x, landerPos.y + 10, landerPos.z);
		cabinCam.setPosition(currentPos.interpolate(targetPos, 0.1));
		cabinCam.lookAt(landerPos + glm::vec3(0, 10, 0) - glm::vec3(glm::rotate(glm::mat4(1.0), glm::radians(landerRotation), glm::vec3(0,1,0)) * glm::vec4(0,0,-1,1)) * 10);
	}

	/* -------------------------------------------------------------------------- */
	/*                                  Emitters                                  */
	/* -------------------------------------------------------------------------- */
	landerPos = lander.getPosition();
	emitter.position = glm::vec3(landerPos.x, landerPos.y - 1, landerPos.z);
	explosionEmitter.position = landerPos;
	
	emitter.update();
	explosionEmitter.update();
	lander.update();

	/* -------------------------------------------------------------------------- */
	/*                                   Physics                                  */
	/* -------------------------------------------------------------------------- */
	float frameRate = ofGetFrameRate();

	if (frameRate > 0) {
		dt = 1.0/frameRate;
	}
	else {
		dt = 0.0f;
	}

	// Linear physics
	landerPos += landerVel * dt;
	landerAccel = (1 / landerMass) * landerForce;
	landerVel += landerAccel * dt;
	landerVel *= damping;

	// Rotation
	landerRotation += angularVel * dt;
	angularAccel = (1 / landerMass) * angularForce;
	angularVel += angularAccel * dt;
	angularVel *= damping;

	// Reset forces
	landerForce = glm::vec3(0, 0, 0);
	angularForce = 0.0f;

	lander.setPosition(landerPos.x, landerPos.y, landerPos.z);
	lander.setRotation(0, landerRotation, 0, 1, 0);

	landerForce += gforce->getForce() * landerMass;
	landerForce += glm::vec3(ofRandom(tforce->getMin().x, tforce->getMax().x), ofRandom(tforce->getMin().y, tforce->getMax().y), ofRandom(tforce->getMin().z, tforce->getMax().z));

	if (thrust) {
		landerForce += glm::vec3(0, 10, 0);
		if (!thrusterOn) {
			emitter.start();
			thrusterSnd.play();
		}
		thrusterOn = true;
	}
	else if (!thrust) {
		if (thrusterOn) {
			emitter.stop();
			emitter.sys->reset();
			thrusterSnd.stop();
		}
		thrusterOn = false;
	}
	if (forward) {
		// landerForce += glm::vec3(glm::rotate(glm::mat4(1.0), glm::radians(landerRotation), glm::vec3(0, 1, 0)) * glm::vec4(1, 0, 0, 1)) * 10;
		landerForce -= glm::vec3(glm::rotate(glm::mat4(1.0), glm::radians(landerRotation), glm::vec3(0,1,0)) * glm::vec4(0,0,-1,1)) * 10;
		forwardSnd.play();
	}
	if (backward) {
		landerForce += glm::vec3(glm::rotate(glm::mat4(1.0), glm::radians(landerRotation), glm::vec3(0,1,0)) * glm::vec4(0,0,-1,1)) * 10;
		backwardSnd.play();
	}
	if (left) {
		angularForce += 50;
		turnSnd.play();
	}
	if (right) {
		angularForce -= 50;
		turnSnd.play();
	}

	// Get Altitude
	if (bAGL) {
		getAGL();
	}

	/* -------------------------------------------------------------------------- */
	/*                                  Collision                                 */
	/* -------------------------------------------------------------------------- */
	ofVec3f min = lander.getSceneMin() + lander.getPosition();
	ofVec3f max = lander.getSceneMax() + lander.getPosition();

	Box bounds = Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));

	colBoxList.clear();
	octree.intersect(bounds, octree.root, colBoxList);

	// Check if in collided state
	if (colBoxList.size() >= 15) {
		// Check explosion
		if (glm::length(landerVel) > 3) {
			// Add explosions
			if (!exploded) {
				explosionEmitter.start();
				explosionSnd.play();
			}
			cout << "Too fast, exploded." << endl; 

			int r1 = 0;
			int r2 = 0;
			if (ofRandom(-1, 1) > 0) r1 = 1;
			else r1 = -1;

			if (ofRandom(-1, 1) > 0) r2 = 1;
			else r2 = -1;

			// landerForce += glm::vec3(r1 * 3000, 3000, r2 * 3000);
			// return;
		}
		else if (glm::length(landerVel) < 3) {
			// Smooth landing
			cout << "Smooth landing." << endl;
			successLandSnd.play();
		}

		// 1. Find lander box center
			glm::vec3 landerBox = glm::vec3(bounds.center().x(), bounds.center().y(), bounds.center().z());

		// 2. Find nearest collided terrain box
		glm::vec3 terrainBox;
		float min = FLT_MAX;
		for (int i = 0; i < colBoxList.size(); i++) {
			Vector3 center = colBoxList[i].center();
			glm::vec3 box = glm::vec3(center.x(), center.y(), center.z());
			float distance = glm::length(landerBox - box);
			if (distance < min) {
				min = distance;
				terrainBox = box;
			}
		}

		// 3. Add bounce
		float bounce = 0.1;

		// 4. Find normal
		glm::vec3 normal = glm::normalize(landerBox - terrainBox);
		glm::vec3 impulse = (bounce + 1) * (-glm::dot(landerVel, normal)) * normal;

		landerVel = impulse;
	}
	if (exploded) {
		explosionEmitter.stop();
		explosionEmitter.sys->reset();
		exploded = false;
	}

}

//--------------------------------------------------------------
void ofApp::draw() {

	/* -------------------------------------------------------------------------- */
	/*                                 Background                                 */
	/* -------------------------------------------------------------------------- */
	ofDisableDepthTest();
	ofSetColor(255);
	ofFill();
	backgroundImg.draw(0, 0, ofGetWindowWidth(), ofGetWindowHeight());
	for (int i = 0; i < 10; i++) {
		ofDrawCircle(ofRandomWidth(), ofRandomHeight(), ofRandom(0.1, 2.5));
	}
	ofEnableDepthTest();

	/* -------------------------------------------------------------------------- */
	/*                                 Explosion                                  */
	/* -------------------------------------------------------------------------- */
	loadVbo();
	loadExplosionVbo();

	/* -------------------------------------------------------------------------- */
	/*                                  Lighting                                  */
	/* -------------------------------------------------------------------------- */
	if (landerLightOn) landerLight.enable();
	else landerLight.disable();

	/* -------------------------------------------------------------------------- */
	/*                                  Music                                     */
	/* -------------------------------------------------------------------------- */
	if (backgroundMusicOn) backgroundSnd.setVolume(0.2f);
	else backgroundSnd.setVolume(0.0f);

	/* -------------------------------------------------------------------------- */
	/*                                  Cameras                                   */
	/* -------------------------------------------------------------------------- */
	if (!activeCam) return;

	/* -------------------------------------------------------------------------- */
	/*                           Draw Terrain and Lander                          */
	/* -------------------------------------------------------------------------- */


	activeCam->begin();
	ofPushMatrix();
	ofEnableLighting();              // shaded mode
	mars.drawFaces();
	ofMesh mesh;
	if (bLanderLoaded) {
		lander.drawFaces();
		if (landerLightOn) {
			landerLight.setPosition(landerPos.x, landerPos.y + 2, landerPos.z);
			ofSetColor(ofColor::lightCoral);
			landerLight.lookAt(ofVec3f(landerPos.x, 2.0, landerPos.z));
			ofDrawSphere(landerLight.getPosition(), 0.4);
		}
		 
		if (!bTerrainSelected) drawAxis(lander.getPosition());
		if (bDisplayBBoxes) {
			ofNoFill();
			ofSetColor(ofColor::white);
			for (int i = 0; i < lander.getNumMeshes(); i++) {
				ofPushMatrix();
				ofMultMatrix(lander.getModelMatrix());
				ofRotate(-90, 1, 0, 0);
				Octree::drawBox(bboxList[i]);
				ofPopMatrix();
			}
		}

		if (bLanderSelected) {

			ofVec3f min = lander.getSceneMin() + lander.getPosition();
			ofVec3f max = lander.getSceneMax() + lander.getPosition();

			Box bounds = Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));
			ofSetColor(ofColor::white);
			Octree::drawBox(bounds);

			// draw colliding boxes
			//
			ofSetColor(ofColor::red);
			for (int i = 0; i < colBoxList.size(); i++) {
				Octree::drawBox(colBoxList[i]);
			}
		}
	}
	if (bTerrainSelected) drawAxis(ofVec3f(0, 0, 0));

	/* -------------------------------------------------------------------------- */
	/*                               Draw Particles                               */
	/* -------------------------------------------------------------------------- */
	glDepthMask(false);

	// this makes everything look glowy :)
	//
	ofEnableBlendMode(OF_BLENDMODE_ADD);
	ofEnablePointSprites();

	// begin drawing in the camera
	//
	shader.begin();
	//activeCam->begin();

	// draw particle emitter here..
	//
	// emitter.draw();
	particleTex.bind();
	ofSetColor(237, 34, 19);
	vbo.draw(GL_POINTS, 0, (int)emitter.sys->particles.size());

	ofSetColor(217, 219, 90);
	explosionVBO.draw(GL_POINTS, 0, (int)explosionEmitter.sys->particles.size());
	particleTex.unbind();

	//  end drawing in the camera
	// 
	//activeCam->end();
	shader.end();

	ofDisablePointSprites();
	ofDisableBlendMode();
	ofEnableAlphaBlending();

	glDepthMask(true);

	// recursively draw octree
	//
	ofDisableLighting();
	int level = 0;
	//	ofNoFill();

	if (bDisplayLeafNodes) {
		octree.drawLeafNodes(octree.root);
		cout << "num leaf: " << octree.numLeaf << endl;
    }
	else if (bDisplayOctree) {
		ofNoFill();
		ofSetColor(ofColor::white);
		octree.draw(numLevels, 0);
	}

	// if point selected, draw a sphere
	//
	// if (pointSelected) {
	// 	ofVec3f p = octree.mesh.getVertex(selectedNode.points[0]);
	// 	ofVec3f d = p - cam.getPosition();
	// 	ofSetColor(ofColor::lightGreen);
	// 	ofDrawSphere(p, .02 * d.length());
	// }

	ofPopMatrix();
	activeCam->end();

	/* -------------------------------------------------------------------------- */
	/*                                   Draw UI                                  */
	/* -------------------------------------------------------------------------- */
	glDepthMask(false);
	if (!bHide) gui.draw();
	if (bAGL) ofDrawBitmapString("Altitude: " + ofToString(altitude, 2), 15 * 2, 60 * 2);
	glDepthMask(true);
}


// 
// Draw an XYZ axis in RGB at world (0,0,0) for reference.
//
void ofApp::drawAxis(ofVec3f location) {

	ofPushMatrix();
	ofTranslate(location);

	ofSetLineWidth(1.0);

	// X Axis
	ofSetColor(ofColor(255, 0, 0));
	ofDrawLine(ofPoint(0, 0, 0), ofPoint(1, 0, 0));
	

	// Y Axis
	ofSetColor(ofColor(0, 255, 0));
	ofDrawLine(ofPoint(0, 0, 0), ofPoint(0, 1, 0));

	// Z Axis
	ofSetColor(ofColor(0, 0, 255));
	ofDrawLine(ofPoint(0, 0, 0), ofPoint(0, 0, 1));

	ofPopMatrix();
}


void ofApp::keyPressed(int key) {

	//Log key press
	ofLogNotice() << "Key pressed: " << key;

	if (key == ' ') {
		thrust = true;
	}
	if (key == OF_KEY_UP) {
    forward = true;
  }
  if (key == OF_KEY_DOWN) {
    backward = true;
  }
  if (key == OF_KEY_LEFT) {
    left = true;
  }
  if (key == OF_KEY_RIGHT) {
    right = true;
  }

  if (key == '1') {
	  switchCam(1);
	  // camName = "Fixed Position Camera (1)";
		camName.set("Fixed Position Camera (1)");
  } else if (key == '2') {
	  switchCam(2);
	  // camName = "Tracking Camera (2)";
		camName.set("Tracking Camera (2)");
		trackCam.setPosition(lander.getPosition().x, lander.getPosition().y + 20, lander.getPosition().z + 45);
		trackCam.lookAt(lander.getPosition());
  } else if (key == '3') {
	  switchCam(3);
	  // camName = "Lander Camera (3)";
		camName.set("Lander Camera (3)");
		landerCam.setPosition(lander.getPosition().x, lander.getPosition().y + 50, lander.getPosition().z);
		landerCam.lookAt(lander.getPosition());
  }
	else if (key == '4') {
		switchCam(4);
		// camName = "Cabin Camera (4)";
		camName.set("Cabin Camera (4)");
		cabinCam.setPosition(lander.getPosition().x, lander.getPosition().y + 10, lander.getPosition().z + 10);
		cabinCam.lookAt(lander.getPosition() + glm::vec3(0, 0, 45));
	}

  //use Dynamic cast for easy camera when it is under camera pointer
  ofEasyCam * easy = dynamic_cast<ofEasyCam *>(activeCam);

	switch (key) {
	case 'A':
	case 'a':
		landerLightOn = !landerLightOn;
		break;
	case 'B':
	case 'b':
		bDisplayBBoxes = !bDisplayBBoxes;
		break;
	case 'C':
	case 'c':
		if (easy) {
			if (easy->getMouseInputEnabled())
				easy->disableMouseInput();
			else
				easy->enableMouseInput();
			cout << mainCam.getPosition() << endl;
			cout << mainCam.getGlobalPosition() << endl;
		}
		break;
	case 'F':
	case 'f':
		ofToggleFullscreen();
		break;
	case 'H':
	case 'h':
		bAGL = !bAGL;
		break;
	case 'L':
	case 'l':
		bDisplayLeafNodes = !bDisplayLeafNodes;
		break;
	case 'M':
	case 'm':
		backgroundMusicOn = !backgroundMusicOn;
		break;
	case 'O':
	case 'o':
		bDisplayOctree = !bDisplayOctree;
		break;

	case 'r':
		if (activeCam) activeCam->resetTransform();
		break;
	case 's':
		savePicture();
		break;
	case 't':
		setCameraTarget();
		break;
	case 'u':
		if (collidedKeyPress) collidedKeyPress = false;
		else collidedKeyPress = true;
		// reverseCollision();
		break;
	case 'v':
		togglePointsDisplay();
		break;
	case 'V':
		break;
	case 'w':
		toggleWireframeMode();
		break;	
	case OF_KEY_ALT:
		 if (easy) {                   // ← check for nullptr
            easy->disableMouseInput();
        }
        bAltKeyDown = false;
        break;
	case OF_KEY_CONTROL:
		bCtrlKeyDown = true;
		break;
	case OF_KEY_SHIFT:
		break;
	case OF_KEY_DEL:
		break;
	default:
		break;
	}

}

void ofApp::toggleWireframeMode() {
	bWireframe = !bWireframe;
}

void ofApp::toggleSelectTerrain() {
	bTerrainSelected = !bTerrainSelected;
}

void ofApp::togglePointsDisplay() {
	bDisplayPoints = !bDisplayPoints;
}

void ofApp::keyReleased(int key) {
	if (key == ' ') {
		thrust = false;
	}
	if (key == OF_KEY_UP) {
    forward = false;
  }
  if (key == OF_KEY_DOWN) {
    backward = false;
  }
  if (key == OF_KEY_LEFT) {
    left = false;
  }
  if (key == OF_KEY_RIGHT) {
    right = false;
  }

  ofEasyCam * easy = dynamic_cast<ofEasyCam *>(activeCam);
	switch (key) {
	
	case OF_KEY_ALT:
		if (easy) {
			easy->disableMouseInput();
		}
		bAltKeyDown = false;
		break;
	case OF_KEY_CONTROL:
		bCtrlKeyDown = false;
		break;
	case OF_KEY_SHIFT:
		break;
	default:
		break;

	}
}



//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

	
}


//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {

	if (dynamic_cast<ofEasyCam *>(activeCam)) {
		return; // EasyCam handles its own mouse input
	}

	//Update Lander Camera look at target where the mousepressed
	//landerCam.lookAt(glm::vec3(x, y, 0));

	// if rover is loaded, test for selection
	//
	if (bLanderLoaded) {
		glm::vec3 origin = activeCam->getPosition();
		glm::vec3 mouseWorld = activeCam->screenToWorld(glm::vec3(mouseX, mouseY, 0));
		glm::vec3 mouseDir = glm::normalize(mouseWorld - origin);

		ofVec3f min = lander.getSceneMin() + lander.getPosition();
		ofVec3f max = lander.getSceneMax() + lander.getPosition();

		Box bounds = Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));
		bool hit = bounds.intersect(Ray(Vector3(origin.x, origin.y, origin.z), Vector3(mouseDir.x, mouseDir.y, mouseDir.z)), 0, 10000);
		if (hit) {
			bLanderSelected = true;
			mouseDownPos = getMousePointOnPlane(lander.getPosition(), activeCam->getZAxis());
			mouseLastPos = mouseDownPos;
			bInDrag = true;
		}
		else {
			bLanderSelected = false;
		}
	}
	else {
		ofVec3f p;
		bool selected = raySelectWithOctree(p);
	}
}

bool ofApp::raySelectWithOctree(ofVec3f &pointRet) {

	if (!activeCam) return false;

	ofVec3f mouse(mouseX, mouseY);
	ofVec3f rayPoint = activeCam->screenToWorld(mouse);
	ofVec3f rayDir = rayPoint - activeCam->getPosition();
	rayDir.normalize();
	Ray ray = Ray(Vector3(rayPoint.x, rayPoint.y, rayPoint.z),
		Vector3(rayDir.x, rayDir.y, rayDir.z));

	float rayStartTime = ofGetElapsedTimeMicros();
	pointSelected = octree.intersect(ray, octree.root, selectedNode);
	float rayEndTime = ofGetElapsedTimeMicros();
	float raySearchTime = rayEndTime - rayStartTime;

	if (pointSelected) {
		pointRet = octree.mesh.getVertex(selectedNode.points[0]);
	}

	return pointSelected;
}




//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {

	// if moving camera, don't allow mouse interaction
	//ofEasyCam * easy = dynamic_cast<ofEasyCam *>(activeCam);
	//if ((easy && easy->getMouseInputEnabled())) return;

	 if (dynamic_cast<ofEasyCam *>(activeCam)) {
		return;
	}

	if (bInDrag) {

		glm::vec3 landerPos = lander.getPosition();

		glm::vec3 mousePos = getMousePointOnPlane(landerPos, activeCam->getZAxis());
		glm::vec3 delta = mousePos - mouseLastPos;
	
		landerPos += delta;
		lander.setPosition(landerPos.x, landerPos.y, landerPos.z);
		mouseLastPos = mousePos;

		ofVec3f min = lander.getSceneMin() + lander.getPosition();
		ofVec3f max = lander.getSceneMax() + lander.getPosition();

		Box bounds = Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));

		colBoxList.clear();
		octree.intersect(bounds, octree.root, colBoxList);

		// Check if in collided state
		collidedState = colBoxList.size() >= 10;
	

		/*if (bounds.overlap(testBox)) {
			cout << "overlap" << endl;
		}
		else {
			cout << "OK" << endl;
		*/


	}
	else {
		ofVec3f p;
		raySelectWithOctree(p);
	}
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {
	if (dynamic_cast<ofEasyCam *>(activeCam)) {
		return;
	}
	bInDrag = false;
}



// Set the camera to use the selected point as it's new target
//  
void ofApp::setCameraTarget() {

}


//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}



/*

//--------------------------------------------------------------
// setup basic ambient lighting in GL  (for now, enable just 1 light)
//
void ofApp::initLightingAndMaterials() {

	static float ambient[] =
	{ .5f, .5f, .5, 1.0f };
	static float diffuse[] =
	{ 1.0f, 1.0f, 1.0f, 1.0f };

	static float position[] =
	{5.0, 5.0, 5.0, 0.0 };

	static float lmodel_ambient[] =
	{ 1.0f, 1.0f, 1.0f, 1.0f };

	static float lmodel_twoside[] =
	{ GL_TRUE };


	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, position);

	glLightfv(GL_LIGHT1, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT1, GL_POSITION, position);


	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
	glLightModelfv(GL_LIGHT_MODEL_TWO_SIDE, lmodel_twoside);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
//	glEnable(GL_LIGHT1);
	glShadeModel(GL_SMOOTH);
}
*/

void ofApp::savePicture() {
	ofImage picture;
	picture.grabScreen(0, 0, ofGetWidth(), ofGetHeight());
	picture.save("screenshot.png");
	cout << "picture saved" << endl;
}

//--------------------------------------------------------------
//
// support drag-and-drop of model (.obj) file loading.  when
// model is dropped in viewport, place origin under cursor
//
void ofApp::dragEvent2(ofDragInfo dragInfo) {

	ofVec3f point;
	mouseIntersectPlane(ofVec3f(0, 0, 0), activeCam->getZAxis(), point);
	if (lander.loadModel(dragInfo.files[0])) {
		lander.setScaleNormalization(false);
//		lander.setScale(.1, .1, .1);
	//	lander.setPosition(point.x, point.y, point.z);
		lander.setPosition(1, 1, 0);

		bLanderLoaded = true;
		for (int i = 0; i < lander.getMeshCount(); i++) {
			bboxList.push_back(Octree::meshBounds(lander.getMesh(i)));
		}

		cout << "Mesh Count: " << lander.getMeshCount() << endl;
	}
	else cout << "Error: Can't load model" << dragInfo.files[0] << endl;
}

bool ofApp::mouseIntersectPlane(ofVec3f planePoint, ofVec3f planeNorm, ofVec3f &point) {
	ofVec2f mouse(mouseX, mouseY);
	ofVec3f rayPoint = activeCam->screenToWorld(glm::vec3(mouseX, mouseY, 0));
	ofVec3f rayDir = rayPoint - activeCam->getPosition();
	rayDir.normalize();
	return (rayIntersectPlane(rayPoint, rayDir, planePoint, planeNorm, point));
}

//--------------------------------------------------------------
//
// support drag-and-drop of model (.obj) file loading.  when
// model is dropped in viewport, place origin under cursor
//
void ofApp::dragEvent(ofDragInfo dragInfo) {
	if (lander.loadModel(dragInfo.files[0])) {
		bLanderLoaded = true;
		lander.setScaleNormalization(false);
		lander.setPosition(0, 0, 0);
		cout << "number of meshes: " << lander.getNumMeshes() << endl;
		bboxList.clear();
		for (int i = 0; i < lander.getMeshCount(); i++) {
			bboxList.push_back(Octree::meshBounds(lander.getMesh(i)));
		}

		//		lander.setRotation(1, 180, 1, 0, 0);

				// We want to drag and drop a 3D object in space so that the model appears 
				// under the mouse pointer where you drop it !
				//
				// Our strategy: intersect a plane parallel to the camera plane where the mouse drops the model
				// once we find the point of intersection, we can position the lander/lander
				// at that location.
				//

				// Setup our rays
				//
		glm::vec3 origin = activeCam->getPosition();
		glm::vec3 camAxis = activeCam->getZAxis();
		glm::vec3 mouseWorld = activeCam->screenToWorld(glm::vec3(mouseX, mouseY, 0));
		glm::vec3 mouseDir = glm::normalize(mouseWorld - origin);
		float distance;

		bool hit = glm::intersectRayPlane(origin, mouseDir, glm::vec3(0, 0, 0), camAxis, distance);
		if (hit) {
			// find the point of intersection on the plane using the distance 
			// We use the parameteric line or vector representation of a line to compute
			//
			// p' = p + s * dir;
			//
			glm::vec3 intersectPoint = origin + distance * mouseDir;

			// Now position the lander's origin at that intersection point
			//
			glm::vec3 min = lander.getSceneMin();
			glm::vec3 max = lander.getSceneMax();
			float offset = (max.y - min.y) / 2.0;
			lander.setPosition(intersectPoint.x, intersectPoint.y - offset, intersectPoint.z);

			// set up bounding box for lander while we are at it
			//
			landerBounds = Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));
		}
	}


}

//  intersect the mouse ray with the plane normal to the camera 
//  return intersection point.   (package code above into function)
//
glm::vec3 ofApp::getMousePointOnPlane(glm::vec3 planePt, glm::vec3 planeNorm) {
	ofEasyCam * easy = dynamic_cast<ofEasyCam *>(activeCam);

	if (!activeCam) return glm::vec3(0, 0, 0);
	// Setup our rays
	//
	glm::vec3 origin = activeCam->getPosition();
	glm::vec3 camAxis = activeCam->getZAxis();
	glm::vec3 mouseWorld = activeCam->screenToWorld(glm::vec3(mouseX, mouseY, 0));
	glm::vec3 mouseDir = glm::normalize(mouseWorld - origin);
	float distance;

	bool hit = glm::intersectRayPlane(origin, mouseDir, planePt, planeNorm, distance);

	if (hit) {
		// find the point of intersection on the plane using the distance 
		// We use the parameteric line or vector representation of a line to compute
		//
		// p' = p + s * dir;
		//
		glm::vec3 intersectPoint = origin + distance * mouseDir;

		return intersectPoint;
	}
	else return glm::vec3(0, 0, 0);
}

void ofApp::reverseCollision() {
	if (collidedState) {
		ofVec3f pos = lander.getPosition();
		pos.y += 0.1;
		lander.setPosition(pos.x, pos.y, pos.z);

		ofVec3f min = lander.getSceneMin() + lander.getPosition();
		ofVec3f max = lander.getSceneMax() + lander.getPosition();
		Box bounds = Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));

		colBoxList.clear();
		octree.intersect(bounds, octree.root, colBoxList);

		// Check if in collided state
		collidedState = colBoxList.size() >= 10;
	}
}

void ofApp::getAGL() {
	glm::vec3 pos = lander.getPosition();
	glm::vec3 dir = glm::vec3(0, -1, 0); // points down
	glm::normalize(dir);
	Ray ray = Ray(Vector3(pos.x, pos.y, pos.z), Vector3(pos.x, pos.y, pos.z));

	TreeNode node;
	octree.intersect(ray, octree.root, selectedNode);
	Vector3 center = node.box.center();
	altitude = glm::distance(lander.getPosition(), glm::vec3(center.x(), center.y(), center.z()));
}

// load vertex buffer in preparation for rendering
//
void ofApp::loadVbo() {
	if (emitter.sys->particles.size() < 1) return;

	vector<ofVec3f> sizes; 
	vector<ofVec3f> points;
	for (int i = 0; i < emitter.sys->particles.size(); i++) {
		points.push_back(emitter.sys->particles[i].position);
		sizes.push_back(ofVec3f(emitter.particleRadius));
	}
	// upload the data to the vbo
	//
	int total = (int)points.size();
	vbo.clear();
	vbo.setVertexData(&points[0], total, GL_STATIC_DRAW);
	vbo.setNormalData(&sizes[0], total, GL_STATIC_DRAW);
}

// load vertex buffer in preparation for rendering
//
void ofApp::loadExplosionVbo() {
	if (explosionEmitter.sys->particles.size() < 1) return;

	vector<ofVec3f> sizes; 
	vector<ofVec3f> points;
	for (int i = 0; i < explosionEmitter.sys->particles.size(); i++) {
		points.push_back(explosionEmitter.sys->particles[i].position);
		sizes.push_back(ofVec3f(explosionEmitter.particleRadius));
	}
	// upload the data to the vbo
	//
	int total = (int)points.size();
	explosionVBO.clear();
	explosionVBO.setVertexData(&points[0], total, GL_STATIC_DRAW);
	explosionVBO.setNormalData(&sizes[0], total, GL_STATIC_DRAW);
}

//Swiching between cameras
void ofApp::switchCam(int n) {

	ofEasyCam * easy = dynamic_cast<ofEasyCam *>(activeCam);
	if (easy) easy->disableMouseInput();

	// enable only the selected one
	if (n == 1) {
		activeCam = &mainCam;
		dynamic_cast<ofEasyCam *>(activeCam)->enableMouseInput();
	}
	if (n == 2) activeCam = &trackCam;
	if (n == 3) activeCam = &landerCam;
	if (n == 4) activeCam = &cabinCam;

	currentCam = n;
}
