#include <scene-evolution/StatsManager.h>
#include <scene-evolution/util.h>

#include <QStringBuilder>
#include <QStringList>
#include <QVariant>

#include <algorithm>

StatsManager::StatsManager()
    : mStatsParsed(false)
{
}

bool StatsManager::parse(DatasetManager* datasetMgr, QProgressBar* progressBar) {
    // Clear old stats
    mLabels = datasetMgr->getLabelNames();

    mLabelNumPixels = std::vector<unsigned long>(mLabels.size(), 0);
    mLabelNumImgs = std::vector<unsigned int>(mLabels.size(), 0);
    mLabelAvgPixels = std::vector<float>(mLabels.size(), 0.);

    // Init progress bar
    progressBar->setValue(0);
    progressBar->setTextVisible(true);

    // Iterate over scenes
    QStringList allScenes = datasetMgr->getAllSceneNames();
    int currentScene = 0;
    for(auto&& sceneName : allScenes) {
        Scene scene = datasetMgr->loadScene(sceneName);
        cv::Mat labelImg = scene.getLabelImg();

        // Collect labels and number of pixels per label
        std::map<unsigned short, unsigned int> labelPixels;
        for(cv::Mat1w::iterator it = labelImg.begin<unsigned short>(); it != labelImg.end<unsigned short>(); ++it) {
            ++labelPixels[*it];
        }

        // Add to global counts
        for(auto&& pair : labelPixels) {
            mLabelNumPixels[pair.first] += pair.second;
            ++mLabelNumImgs[pair.first];
        }

        // Set progress
        ++currentScene;
        progressBar->setValue(currentScene * 100 / allScenes.size());
    }

    // Calculate averages
    for(size_t i = 0; i < mLabelAvgPixels.size(); ++i) {
        mLabelAvgPixels[i] = static_cast<double>(mLabelNumPixels[i]) / static_cast<double>(mLabelNumImgs[i]);
    }

    // Reset progress bar
    progressBar->setValue(0);
    progressBar->setTextVisible(false);

    // Set stats parsed
    mStatsParsed = true;

    return true;
}

QString StatsManager::print(unsigned short labelIdx, PrintFlags pflags) const {
    if(!mStatsParsed)
        return QString();

    return buildString(labelIdx, pflags);
}

QString StatsManager::print(const std::vector<unsigned short>& labelIdxs, PrintFlags pflags) const {
    if(!mStatsParsed)
        return QString();

    QString ret;

    for(auto&& labelIdx : labelIdxs)
        ret += buildString(labelIdx, pflags) % "\n";

    return ret;
}

QString StatsManager::print(unsigned short topX, SortBy sortBy, SortFlags sflags, PrintFlags pflags) const {
    if(!mStatsParsed)
        return QString();

    QString ret;

    // Get top x labels sorted according to the parameters
    if(sortBy == SORT_AVG_PX_PER_IMG) {
        std::vector<std::pair<unsigned short, float>> avgs = sortForPrint(topX, mLabelAvgPixels, sflags);

        for(unsigned int i = 0; i < topX && i < avgs.size(); ++i)
            ret += buildString(avgs[i].first, pflags);
    }
    else if(sortBy == SORT_TOTAL_NUM_IMG) {
        std::vector<std::pair<unsigned short, unsigned int>> numImgs = sortForPrint(topX, mLabelNumImgs, sflags);

        for(unsigned int i = 0; i < topX && i < numImgs.size(); ++i)
            ret += buildString(numImgs[i].first, pflags);
    }
    else if(sortBy == SORT_TOTAL_NUM_PX) {
        std::vector<std::pair<unsigned short, unsigned long>> numPx = sortForPrint(topX, mLabelNumPixels, sflags);

        for(unsigned int i = 0; i < topX && i < numPx.size(); ++i)
            ret += buildString(numPx[i].first, pflags);
    }

    return ret;
}

QString StatsManager::print(SortBy sortBy, SortFlags sflags, PrintFlags pflags) const {
    if(!mStatsParsed)
        return QString();


    return print(mLabels.size(), sortBy, sflags, pflags);
}

QString StatsManager::buildString(unsigned short labelIdx, PrintFlags pflags) const {
    QString ret = mLabels[labelIdx] % ":\n";

    if((pflags & PRINT_AVG_PX_PER_IMG) == PRINT_AVG_PX_PER_IMG)
        ret += Strings::StatsDisplayLinePrefix % "Avg. pixels per scene: " % QString::number(mLabelAvgPixels[labelIdx]) % "\n";

    if((pflags & PRINT_TOTAL_NUM_IMG) == PRINT_TOTAL_NUM_IMG)
        ret += Strings::StatsDisplayLinePrefix % "Total number of scenes: " % QString::number(mLabelNumImgs[labelIdx]) % "\n";

    if((pflags & PRINT_TOTAL_NUM_PX) == PRINT_TOTAL_NUM_PX)
        ret += Strings::StatsDisplayLinePrefix % "Total number of pixels: " % QString::number(mLabelNumPixels[labelIdx]) % "\n";

    return ret;
}

template<typename T>
std::vector<std::pair<unsigned short, T>> StatsManager::sortForPrint(unsigned short topX, const std::vector<T>& vec, SortFlags sflags) const {
    std::vector<std::pair<unsigned short, T>> ret;
    ret.reserve(vec.size());
    for(size_t i = 0; i < vec.size(); ++i)
        ret.emplace_back(i, vec[i]);

    auto asc = [](const std::pair<unsigned short, T>& lhs, const std::pair<unsigned short, T>& rhs) -> bool {
        return lhs.second < rhs.second;
    };
    auto desc = [](const std::pair<unsigned short, T>& lhs, const std::pair<unsigned short, T>& rhs) -> bool {
        return lhs.second > rhs.second;
    };

    if(topX < ret.size())
        std::partial_sort(ret.begin(), ret.begin() + topX, ret.end(), ((sflags & SORT_ASC) == SORT_ASC ? asc : desc));
    else
        std::sort(ret.begin(), ret.end(), ((sflags & SORT_ASC) == SORT_ASC ? asc : desc));

    return ret;
}
