#include <scene-evolution/LabelOverlayManager.h>
#include <scene-evolution/util.h>

#include <OGRE/Overlay/OgreOverlayManager.h>
#include <OGRE/Overlay/OgreOverlay.h>
#include <OGRE/Overlay/OgreOverlayContainer.h>
#include <OGRE/Overlay/OgrePanelOverlayElement.h>
#include <OGRE/Overlay/OgreTextAreaOverlayElement.h>
#include <OGRE/OgreMaterial.h>
#include <OGRE/OgreTexture.h>
#include <OGRE/OgreHardwarePixelBuffer.h>

#include <QImage>
#include <QPainter>

LabelOverlayManager::LabelOverlayManager(const Scene& currentScene, unsigned int currentSceneIdx, RGBDScene* currentRGBDScene,
                                         const QVector<QString>& labels, unsigned int minLabelPx, unsigned int fontSize,
                                         Ogre::Camera* ogreCamera, unsigned int windowWidth, unsigned int windowHeight)
    : mCurrentSceneIdx(currentSceneIdx)
    , mCurrentScene(currentScene)
    , mCurrentRGBDScene(currentRGBDScene)
    , mCamera(ogreCamera)
    , mLabels(labels)
    , mLabelsEnabled(false)
    , mMinPx(minLabelPx)
    , mFontSize(fontSize)
    , mLabelBordersEnabled(false)
    , mWindowWidth(windowWidth)
    , mWindowHeight(windowHeight)
{
}

LabelOverlayManager::~LabelOverlayManager() {
    // TODO: destroy everything in the maps
}

void LabelOverlayManager::windowResized(Ogre::RenderWindow* rw) {
    mWindowWidth = rw->getWidth();
    mWindowHeight = rw->getHeight();

    if(mLabelsEnabled)
        updateLabels(true);

    if(mLabelBordersEnabled)
        updateLabelBorders(true);
}

void LabelOverlayManager::onDatasetChangingConfirmed(DatasetChangingConfirmedEventArgs& e) {
    Q_UNUSED(e);

    Ogre::OverlayManager& overlayMgr = Ogre::OverlayManager::getSingleton();

    // Clear overlay map
    for(SceneOverlayMap::iterator it = mSceneOverlayMap.begin(); it != mSceneOverlayMap.end(); ++it) {
        Ogre::Overlay::Overlay2DElementsIterator overlayIt = std::get<0>(*it)->get2DElementsIterator();
        while(overlayIt.hasMoreElements()) {
            Ogre::OverlayContainer* panel = overlayIt.getNext();
            Ogre::OverlayContainer::ChildIterator panelIt = panel->getChildIterator();
            while(panelIt.hasMoreElements())
                overlayMgr.destroyOverlayElement(panelIt.getNext());
            overlayMgr.destroyOverlayElement(panel);
        }
        overlayMgr.destroy(std::get<0>(*it));
    }
    mSceneOverlayMap.clear();

    // Clear border map
    for(SceneBorderMap::iterator it = mSceneBorderMap.begin(); it != mSceneBorderMap.end(); ++it) {
        Ogre::Overlay::Overlay2DElementsIterator overlayIt = std::get<0>(*it)->get2DElementsIterator();
        while(overlayIt.hasMoreElements()) {
            Ogre::OverlayContainer* panel = overlayIt.getNext();
            overlayMgr.destroyOverlayElement(panel);
        }
        overlayMgr.destroy(std::get<0>(mSceneBorderMap[mCurrentSceneIdx]));
    }
    mSceneBorderMap.clear();

    // Clear maps map
    mSceneMapsMap.clear();

    // Save new labels
    mLabels = e.labels;
}

void LabelOverlayManager::onSceneChanged(SceneChangedEventArgs& e) {
    // Hide labels of current scene
    if(mSceneOverlayMap.contains(mCurrentSceneIdx))
        std::get<0>(mSceneOverlayMap[mCurrentSceneIdx])->hide();

    // Hide borders of current scene
    if(mSceneBorderMap.contains(mCurrentSceneIdx))
        std::get<0>(mSceneBorderMap[mCurrentSceneIdx])->hide();

    // Change scene
    mCurrentSceneIdx = e.sceneIdx;
    mCurrentScene = e.scene;
    mCurrentRGBDScene = e.rgbdScene;

    // Update labels to show new scene
    if(mLabelsEnabled)
        updateLabels();

    if(mLabelBordersEnabled)
        updateLabelBorders();
}

void LabelOverlayManager::onCheckBoxDisplayLabelsStateChanged(int state) {
    mLabelsEnabled = (state != Qt::Unchecked);
    if(mLabelsEnabled)
        updateLabels();
    else if(mSceneOverlayMap.contains(mCurrentSceneIdx))
        std::get<0>(mSceneOverlayMap[mCurrentSceneIdx])->hide();
}

void LabelOverlayManager::onSpinBoxMinLabelPxValueChanged(int value) {
    mMinPx = static_cast<unsigned int>(std::max(1, value));
    if(mLabelsEnabled)
        updateLabels();
}

void LabelOverlayManager::onHorizontalSliderLabelFontSizeValueChanged(int value) {
    mFontSize = static_cast<unsigned int>(std::max(1, value));
    if(mLabelsEnabled)
        updateLabels();
}

void LabelOverlayManager::onCheckBoxDisplayLabelBordersStateChanged(int state) {
    mLabelBordersEnabled = (state != Qt::Unchecked);
    if(mLabelBordersEnabled)
        updateLabelBorders();
    else if(mSceneBorderMap.contains(mCurrentSceneIdx))
        std::get<0>(mSceneBorderMap[mCurrentSceneIdx])->hide();
}

void LabelOverlayManager::updateLabels(bool forceRecreate) {
    Ogre::OverlayManager& overlayMgr = Ogre::OverlayManager::getSingleton();

    // Check if labels have to be created
    if(forceRecreate || !mSceneOverlayMap.contains(mCurrentSceneIdx)
                     || std::get<1>(mSceneOverlayMap[mCurrentSceneIdx]) != mMinPx
                     || std::get<2>(mSceneOverlayMap[mCurrentSceneIdx]) != mFontSize
                     || std::get<3>(mSceneOverlayMap[mCurrentSceneIdx]) != mWindowWidth
                     || std::get<4>(mSceneOverlayMap[mCurrentSceneIdx]) != mWindowHeight) {
        // Destroy labels if outdated ones are already present
        if(mSceneOverlayMap.contains(mCurrentSceneIdx)) {
            Ogre::Overlay::Overlay2DElementsIterator overlayIt = std::get<0>(mSceneOverlayMap[mCurrentSceneIdx])->get2DElementsIterator();
            while(overlayIt.hasMoreElements()) {
                Ogre::OverlayContainer* panel = overlayIt.getNext();
                Ogre::OverlayContainer::ChildIterator panelIt = panel->getChildIterator();
                while(panelIt.hasMoreElements())
                    overlayMgr.destroyOverlayElement(panelIt.getNext());
                overlayMgr.destroyOverlayElement(panel);
            }
            overlayMgr.destroy(std::get<0>(mSceneOverlayMap[mCurrentSceneIdx]));
        }

        // Retrieve screen coordinates of the background scene
        Ogre::Vector2 sceneTopLeft, sceneBottomRight;
        {
            CameraLocker lock(mCamera);
            if(!mCurrentRGBDScene->screenspaceCoords(mCamera, sceneTopLeft, sceneBottomRight))
                return;
        }

        // Create new labels
        std::string sceneName = mCurrentScene.getFileName().toStdString();
        cv::Mat labelImg = mCurrentScene.getLabelImg();

        Ogre::Overlay* overlay = overlayMgr.create(sceneName + "Overlay");
        overlay->setZOrder(Constants::OverlayLabelsZOrder);
        // If there is a destroyed overlay pointer left over, insert replaces it
        mSceneOverlayMap.insert(mCurrentSceneIdx, std::make_tuple(overlay, mMinPx, mFontSize, mWindowWidth, mWindowHeight));

        // Retrieve all regions to be labeled by performing region growing on the label image
        // Check if the maps have already been created, if not create them
        if(!mSceneMapsMap.contains(mCurrentSceneIdx))
            createCurrentMaps();

        // Iterate over the cluster map
        for(auto&& pair : mSceneMapsMap.value(mCurrentSceneIdx).second) {
            // Check if this region contains at least as many pixels as the minimum pixel threshold
            if(pair.second.size() >= mMinPx) {

                // Calculate centroid of region
                cv::Point centroid(0, 0);
                for(auto&& point : pair.second)
                    centroid += point;
                centroid.x /= pair.second.size();
                centroid.y /= pair.second.size();

                const QString& label = mLabels[labelImg.at<unsigned short>(*pair.second.begin())];

                // Check if centroid is still on top of the correct label (adjust color if it's not)
                Ogre::ColourValue labelColor(0.f, 0.8f, 0.f);
                if(labelImg.at<unsigned short>(centroid) != labelImg.at<unsigned short>(*pair.second.begin()))
                    labelColor.r = 1.f;

                // Retrieve top left point of label (depending on label text length)
                cv::Point labelTopLeft(centroid.x - 3 * label.length() * mFontSize / 25., centroid.y - mFontSize / 2);

                // Calculate screen coordinate equivalent to top left point of label
                Ogre::Vector2 labelScreenTopLeft(
                    sceneTopLeft.x + (static_cast<Ogre::Real>(labelTopLeft.x) / static_cast<Ogre::Real>(labelImg.cols)) * (sceneBottomRight.x - sceneTopLeft.x),
                    sceneTopLeft.y + (static_cast<Ogre::Real>(labelTopLeft.y) / static_cast<Ogre::Real>(labelImg.rows)) * (sceneBottomRight.y - sceneTopLeft.y)
                );

                // Create overlay panel and text area, then attach them to the current scene's overlay
                Ogre::PanelOverlayElement* panel = static_cast<Ogre::PanelOverlayElement*>(overlayMgr.createOverlayElement(
                                                                                               "Panel",
                                                                                               sceneName + "Panel" + QString::number(pair.first).toStdString())
                                                                                           );
                overlay->add2D(panel);
                Ogre::TextAreaOverlayElement* textArea = static_cast<Ogre::TextAreaOverlayElement*>(overlayMgr.createOverlayElement(
                                                                                                        "TextArea",
                                                                                                        sceneName + "TextArea" + QString::number(pair.first).toStdString())
                                                                                                    );
                panel->addChild(textArea);

                // Set properties of overlay panel (position, size)
                panel->setMetricsMode(Ogre::GMM_RELATIVE);
                panel->setTransparent(true);
                panel->setPosition(labelScreenTopLeft.x, labelScreenTopLeft.y);
                panel->setDimensions(6 * label.length() * mFontSize / 25., mFontSize);

                // Set properties of text area (local position, font, font size, colour, text)
                textArea->setMetricsMode(Ogre::GMM_PIXELS);
                textArea->setPosition(0, 0);
                textArea->setDimensions(500, mFontSize);
                textArea->setFontName(Strings::LabelOverlayFontName);
                textArea->setCharHeight(mFontSize);
                textArea->setSpaceWidth(6);
                textArea->setColour(labelColor);

                textArea->setCaption(label.toStdString());

//                std::cout << "Created label: " << label.toStdString() << '\n'
//                          << "    Position: " << labelScreenTopLeft << '\n'
//                          << "    Dimensions: (" << 7 * label.length() << ", " << mFontSize << ")\n"
//                          << "    Font Name: " << Strings::LabelOverlayFontName << '\n' << std::endl;
            }
        }
    }

    // Show/hide labels
    if(mLabelsEnabled) {
        std::get<0>(mSceneOverlayMap[mCurrentSceneIdx])->show();
    }
    else {
        std::get<0>(mSceneOverlayMap[mCurrentSceneIdx])->hide();
    }
}

void LabelOverlayManager::updateLabelBorders(bool forceRecreate) {
    Ogre::OverlayManager& overlayMgr = Ogre::OverlayManager::getSingleton();

    // Check if label borders have to be created
    if(forceRecreate || !mSceneBorderMap.contains(mCurrentSceneIdx)
                     || std::get<3>(mSceneBorderMap[mCurrentSceneIdx]) != mWindowWidth
                     || std::get<4>(mSceneBorderMap[mCurrentSceneIdx]) != mWindowHeight) {
        // Destroy label borders if outdated ones are already present
        if(mSceneBorderMap.contains(mCurrentSceneIdx)) {
            Ogre::Overlay::Overlay2DElementsIterator overlayIt = std::get<0>(mSceneBorderMap[mCurrentSceneIdx])->get2DElementsIterator();
            while(overlayIt.hasMoreElements()) {
                Ogre::OverlayContainer* panel = overlayIt.getNext();
                overlayMgr.destroyOverlayElement(panel);
            }
            overlayMgr.destroy(std::get<0>(mSceneBorderMap[mCurrentSceneIdx]));

            // Release shared pointers to material and texture to ensure they are actually unloaded
            std::get<1>(mSceneBorderMap[mCurrentSceneIdx]).setNull();
            std::get<2>(mSceneBorderMap[mCurrentSceneIdx]).setNull();
        }

        // Retrieve screen coordinates of the background scene
        Ogre::Vector2 sceneTopLeft, sceneBottomRight;
        {
            CameraLocker lock(mCamera);
            if(!mCurrentRGBDScene->screenspaceCoords(mCamera, sceneTopLeft, sceneBottomRight))
                return;
        }

        // Create new label borders
        std::string sceneName = mCurrentScene.getFileName().toStdString();
        cv::Mat labelImg = mCurrentScene.getLabelImg();

        Ogre::Overlay* overlay = overlayMgr.create(sceneName + "BorderOverlay");
        overlay->setZOrder(Constants::OverlayLabelBordersZOrder);

        // Create texture
        Ogre::TexturePtr texture = Ogre::TextureManager::getSingletonPtr()->createManual(
                                       sceneName + "OverlayTexture",
                                       Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
                                       Ogre::TEX_TYPE_2D,
                                       static_cast<unsigned int>(labelImg.cols),
                                       static_cast<unsigned int>(labelImg.rows),
                                       0,
                                       Ogre::PF_A8R8G8B8);

        // Create material using the texture
        Ogre::MaterialPtr material = Ogre::MaterialManager::getSingletonPtr()->create(sceneName + "OverlayMaterial", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
        Ogre::Pass* pass = material->getTechnique(0)->getPass(0);
        pass->createTextureUnitState(sceneName + "OverlayTexture");
        pass->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);

        // Insert into map
        // If there is a destroyed overlay pointer left over, insert replaces it
        mSceneBorderMap.insert(mCurrentSceneIdx, std::make_tuple(overlay, material, texture, mWindowWidth, mWindowHeight));

        // Create overlay panel, then attach it to the current scene's overlay
        Ogre::PanelOverlayElement* panel = static_cast<Ogre::PanelOverlayElement*>(overlayMgr.createOverlayElement(
                                                                                       "Panel",
                                                                                       sceneName + "PanelBorders")
                                                                                   );
        overlay->add2D(panel);

        // Set properties of overlay panel (position, size, material)
        panel->setMetricsMode(Ogre::GMM_RELATIVE);
        panel->setPosition(sceneTopLeft.x, sceneTopLeft.y);
        panel->setDimensions(sceneBottomRight.x - sceneTopLeft.x, sceneBottomRight.y - sceneTopLeft.y);
        panel->setMaterialName(sceneName + "OverlayMaterial");

        // Paint the borders into the texture
        // Get pixel buffer
        Ogre::HardwarePixelBufferSharedPtr pxBuf = texture->getBuffer();

        // Lock pixel buffer and get a pixel box
        pxBuf->lock(Ogre::HardwareBuffer::HBL_NORMAL);
        const Ogre::PixelBox& pxBox = pxBuf->getCurrentLock();

        Ogre::uint8* pDest = static_cast<Ogre::uint8*>(pxBox.data);

        // Construct border image directly into the buffer
        {
            // Create QImage with existing buffer and a compatible image buffer format
            QImage borderImg(pDest, texture->getWidth(), texture->getHeight(), QImage::Format_ARGB32);

            // Fill with transparency
            borderImg.fill(Qt::transparent);

            // Create QPainter to draw the borders directly into the texture
            QPainter painter(&borderImg);
            painter.setPen(Qt::red);

            // Iterate over label image
            for(int y = 0; y < labelImg.rows - 1; ++y) {
                for(int x = 0; x < labelImg.cols - 1; ++x) {
                    if(labelImg.at<unsigned short>(y, x) != labelImg.at<unsigned short>(y + 1, x)
                            || labelImg.at<unsigned short>(y, x) != labelImg.at<unsigned short>(y, x + 1)) {
                        // Draw point where the label changes
                        painter.drawPoint(x, y);
                    }
                }
            }
        }

        // Unlock buffer
        pxBuf->unlock();
    }

    // Show/hide label borders
    if(mLabelBordersEnabled) {
        std::get<0>(mSceneBorderMap[mCurrentSceneIdx])->show();
    }
    else {
        std::get<0>(mSceneBorderMap[mCurrentSceneIdx])->hide();
    }
}

void LabelOverlayManager::createCurrentMaps() {
    const cv::Mat& labelImg = mCurrentScene.getLabelImg();

    // First create the region map with all points marked as unassigned (-1)
    RegionMap regionMap([](const cv::Point& lhs, const cv::Point& rhs) -> bool {
        return lhs.y == rhs.y ? lhs.x < rhs.x : lhs.y < rhs.y;
    });

    for(cv::Mat1w::const_iterator it = labelImg.begin<unsigned short>(); it != labelImg.end<unsigned short>(); ++it) {
        if(*it > 0) regionMap.emplace(it.pos(), -1);
    }

    // Execute the region growing, resulting in 2 maps: 1) the region map filled with assigned regions, 2) a cluster map storing all points associated with a specific region
    ClusterMap clusterMap = doRegionGrowing(regionMap);

    // Insert the resulting maps into the scene->maps map
    mSceneMapsMap.insert(mCurrentSceneIdx, std::make_pair(regionMap, clusterMap));
}

LabelOverlayManager::ClusterMap LabelOverlayManager::doRegionGrowing(RegionMap& points) const {
    ClusterMap ret;

    int currentId = 0;
    std::queue<RegionMap::iterator> queue;

    // Iterate over all inliers
    for(RegionMap::iterator it = points.begin(); it != points.end(); ++it) {
        // Check if this point was already processed
        if(it->second != -1)
            continue;

        // Add point to the queue
        queue.push(it);

        // Add new cluster for this region (necessary to do this manually in order to specify comparison function)
        ret.emplace(currentId, std::set<cv::Point, bool(*)(const cv::Point&, const cv::Point&)>(
                        [](const cv::Point& lhs, const cv::Point& rhs) -> bool {
                            return lhs.y == rhs.y ? lhs.x < rhs.x : lhs.y < rhs.y;
                        }
        ));

        // Iterate until the whole region is marked
        while(!queue.empty()) {
            RegionMap::iterator current = queue.front();
            queue.pop();

            if(current->second != -1)
                continue;

            // This point is unmarked, so mark it with the current region ID
            current->second = currentId;

            // Add this points to the cluster of the current ID
            ret.at(currentId).insert(current->first);

            // Add all points to the queue that are direct neighbors (8-connected grid) of the current point
            // and have the same label
            cv::Point pixel = current->first;
            cv::Mat labelImg = mCurrentScene.getLabelImg();
            for(int y = pixel.y - 1; y <= pixel.y + 1; ++y) {
                for(int x = pixel.x - 1; x <= pixel.x + 1; ++x) {
                    RegionMap::iterator neighbor = points.find(cv::Point(x, y));
                    // The current point itself will not be added since it is no longer marked with -1
                    if(neighbor != points.cend() && neighbor->second == -1
                            && labelImg.at<unsigned short>(neighbor->first) == labelImg.at<unsigned short>(pixel)) {
                        queue.push(neighbor);
                    }
                }
            }
        }

        ++currentId;
    }

    return ret;
}

LabelOverlayManager::CameraLocker::CameraLocker(Ogre::Camera* camera)
    : mCamera(camera)
    , mOrigPos(camera->getPosition())
    , mOrigOrient(camera->getOrientation())
{
    mCamera->setPosition(Constants::InitialCameraPosition);
    mCamera->lookAt(Constants::InitialCameraLookAt);
}

LabelOverlayManager::CameraLocker::~CameraLocker() {
    mCamera->setPosition(mOrigPos);
    mCamera->setOrientation(mOrigOrient);
}
