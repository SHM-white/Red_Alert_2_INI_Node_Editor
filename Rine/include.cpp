#include "include.h"

int VIEW_DISPLAY_DEPTH = 2;
std::vector<ViewContent> Utils::GetRelatedContents(const ViewContent* content, const std::vector<ViewContent>& contents,int currentDepth)
{
    std::vector<ViewContent> relatedContents;
    relatedContents.push_back(*content);
    //Find parent
    if (currentDepth == 0) {
        for (auto& c : contents) {
            for (auto& i : c.content()) {
                if (content->title().toLower() == i.second.toLower()) {
                    relatedContents.push_back(c);
                    break;
                }
            }
        }
    }
    //Find child
    for (auto& i : content->content()) {
        for (auto& c : contents) {
            if (i.second.toLower() == c.title().toLower()) {
                relatedContents.push_back(c);
                if (currentDepth < VIEW_DISPLAY_DEPTH) {
                    auto childRelatedContents = GetRelatedContents(&c, contents, currentDepth + 1);
                    relatedContents.insert(relatedContents.end(), childRelatedContents.begin(), childRelatedContents.end());
                }
                break;
            }
        }
    }
    return relatedContents;
}

std::vector<ViewContent> Utils::RemoveRepeatedContents(const std::vector<ViewContent>& contents)
{
    std::vector<ViewContent> result;
    std::unordered_set<QString> titles;
    for (auto& c : contents) {
        if (titles.find(c.title()) == titles.end()) {
            result.push_back(c);
            titles.insert(c.title());
        }
    }
    return result;
}
