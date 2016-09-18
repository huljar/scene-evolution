#ifndef STATSMANAGER_H
#define STATSMANAGER_H

#include <scene-evolution/DatasetManager.h>

#include <QProgressBar>
#include <QString>
#include <QVector>

#include <map>
#include <vector>

class StatsManager
{
public:
    enum PrintFlags {
        PRINT_AVG_PX_PER_IMG = 1 << 0,
        PRINT_TOTAL_NUM_IMG = 1 << 1,
        PRINT_TOTAL_NUM_PX = 1 << 2
    };

    enum SortBy {
        SORT_AVG_PX_PER_IMG = 0,
        SORT_TOTAL_NUM_IMG = 1,
        SORT_TOTAL_NUM_PX = 2
    };

    enum SortFlags {
        SORT_ASC = 1 << 0,
        SORT_DESC = 1 << 1
    };

    StatsManager();

    bool parse(DatasetManager* datasetMgr, QProgressBar* progressBar);

    QString print(unsigned short labelIdx, PrintFlags pflags) const;
    QString print(const std::vector<unsigned short>& labelIdxs, PrintFlags pflags) const;
    QString print(unsigned short topX, SortBy sortBy, SortFlags sflags, PrintFlags pflags) const;
    QString print(SortBy sortBy, SortFlags sflags, PrintFlags pflags) const;

protected:
    QVector<QString> mLabels;

    std::vector<unsigned long> mLabelNumPixels;
    std::vector<unsigned int> mLabelNumImgs;

    std::vector<float> mLabelAvgPixels;

    bool mStatsParsed;

private:
    QString buildString(unsigned short labelIdx, PrintFlags flags) const;

    template<typename T>
    std::vector<std::pair<unsigned short, T>> sortForPrint(unsigned short topX, const std::vector<T>& vec, SortFlags sflags) const;
};

inline StatsManager::PrintFlags operator|(StatsManager::PrintFlags lhs, StatsManager::PrintFlags rhs) {
    return static_cast<StatsManager::PrintFlags>(static_cast<int>(lhs) | static_cast<int>(rhs));
}

#endif // STATSMANAGER_H
