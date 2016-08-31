#include <scene-evolution/LabelOverlayManager.h>
#include <scene-evolution/util.h>

#include <OGRE/Overlay/OgreOverlayManager.h>
#include <OGRE/Overlay/OgreOverlay.h>
#include <OGRE/Overlay/OgreOverlayContainer.h>
#include <OGRE/Overlay/OgrePanelOverlayElement.h>
#include <OGRE/Overlay/OgreTextAreaOverlayElement.h>

#include <cmath>

LabelOverlayManager::LabelOverlayManager(const Scene& currentScene, unsigned int currentSceneIdx, int minLabelPx)
    : mCurrentSceneIdx(currentSceneIdx)
    , mCurrentScene(currentScene)
    , mLabelsEnabled(false)
    , mMinPx(minLabelPx)
{
}

void LabelOverlayManager::onDatasetChanged(DatasetChangedEventArgs& e) {
    Q_UNUSED(e);

    // Clear overlay map
    Ogre::OverlayManager& overlayMgr = Ogre::OverlayManager::getSingleton();

    for(SceneOverlayMap::iterator it = mSceneOverlayMap.begin(); it != mSceneOverlayMap.end(); ++it) {
        Ogre::Overlay::Overlay2DElementsIterator overlayIt = it->first->get2DElementsIterator();
        while(overlayIt.hasMoreElements()) {
            Ogre::OverlayContainer* panel = overlayIt.getNext();
            Ogre::OverlayContainer::ChildIterator panelIt = panel->getChildIterator();
            while(panelIt.hasMoreElements())
                overlayMgr.destroyOverlayElement(panelIt.getNext());
            overlayMgr.destroyOverlayElement(panel);
        }
        overlayMgr.destroy(it->first);
    }

    mSceneOverlayMap.clear();
}

void LabelOverlayManager::onSceneChanged(SceneChangedEventArgs& e) {
    // Hide labels of current scene
    if(mSceneOverlayMap.contains(mCurrentSceneIdx))
        mSceneOverlayMap[mCurrentSceneIdx].first->hide();

    // Change scene
    mCurrentSceneIdx = e.sceneIdx;
    mCurrentScene = e.scene;

    // Update labels to show new scene
    updateLabels();
}

void LabelOverlayManager::onCheckBoxDisplayLabelsStateChanged(int state) {
    mLabelsEnabled = (state != Qt::Unchecked);
    updateLabels();
}

void LabelOverlayManager::onSpinBoxMinLabelPxValueChanged(int value) {
    mMinPx = static_cast<unsigned int>(std::max(1, value));
    updateLabels();
}

void LabelOverlayManager::updateLabels(bool forceRecreate) {
    Ogre::OverlayManager& overlayMgr = Ogre::OverlayManager::getSingleton();

    // Check if labels have to be created
    if(forceRecreate || !mSceneOverlayMap.contains(mCurrentSceneIdx) || mSceneOverlayMap[mCurrentSceneIdx].second != mMinPx) {
        // Destroy labels if outdated ones are already present
        if(mSceneOverlayMap.contains(mCurrentSceneIdx)) {
            Ogre::Overlay::Overlay2DElementsIterator overlayIt = mSceneOverlayMap[mCurrentSceneIdx].first->get2DElementsIterator();
            while(overlayIt.hasMoreElements()) {
                Ogre::OverlayContainer* panel = overlayIt.getNext();
                Ogre::OverlayContainer::ChildIterator panelIt = panel->getChildIterator();
                while(panelIt.hasMoreElements())
                    overlayMgr.destroyOverlayElement(panelIt.getNext());
                overlayMgr.destroyOverlayElement(panel);
            }
            overlayMgr.destroy(mSceneOverlayMap[mCurrentSceneIdx].first);
        }

        // Create new labels
        std::string sceneName = mCurrentScene.getFileName().toStdString();
        Ogre::Overlay* overlay = overlayMgr.create(sceneName + "Overlay");
        mSceneOverlayMap.insert(mCurrentSceneIdx, std::make_pair(overlay, mMinPx)); // If there is a destroyed overlay pointer left over, insert replaces it

        // TODO: region growing, get min px value, loop over labels and create panel + text area for each of them
        Ogre::PanelOverlayElement* panel = static_cast<Ogre::PanelOverlayElement*>(overlayMgr.createOverlayElement(
                                                                                       "Panel",
                                                                                       sceneName + "Panel" + QString::number(1).toStdString())
                                                                                   );
        overlay->add2D(panel);
        Ogre::TextAreaOverlayElement* textArea = static_cast<Ogre::TextAreaOverlayElement*>(overlayMgr.createOverlayElement(
                                                                                                "TextArea",
                                                                                                sceneName + "TextArea" + QString::number(1).toStdString())
                                                                                            );
        panel->addChild(textArea);

        panel->setMetricsMode(Ogre::GMM_RELATIVE);
        panel->setTransparent(true);
        panel->setPosition(0.4, 0.4);
        panel->setDimensions(0.2, 0.2);

        textArea->setMetricsMode(Ogre::GMM_PIXELS);
        textArea->setPosition(15, 15);
        textArea->setDimensions(200, 20);
        textArea->setFontName(Strings::LabelOverlayFontName);
        textArea->setCharHeight(19);
        textArea->setSpaceWidth(6);
        textArea->setColour(Ogre::ColourValue::Green);

        textArea->setCaption("Test Overlay");

        std::cout << "Created labels" << std::endl;
    }

    // Show/hide labels
    if(mLabelsEnabled) {
        mSceneOverlayMap[mCurrentSceneIdx].first->show();
        std::cout << "Showing labels" << std::endl;
    }
    else {
        mSceneOverlayMap[mCurrentSceneIdx].first->hide();
        std::cout << "Hiding labels" << std::endl;
    }
}
