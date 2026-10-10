#include "GraphNeighborhood.h"
#include <QSet>
#include <algorithm>
#include <utility>

GraphNeighborhood buildNeighborhood(const IniDocument& document, int focus,
                                     int parentDepth, int childDepth,
                                     int maxPerLevel, int maxPerSide) {
    GraphNeighborhood result;
    if (focus < 0 || focus >= document.sections().size()) return result;
    parentDepth = std::clamp(parentDepth, 0, 5);
    childDepth = std::clamp(childDepth, 0, 5);
    maxPerLevel = std::max(0, maxPerLevel);
    maxPerSide = std::max(0, maxPerSide);
    const auto traverse = [&](bool upstream, int maxDepth,
                              QVector<GraphNeighborhood::Node>& nodes,
                              bool& truncated) {
        QSet<int> seen{focus};
        QVector<int> frontier{focus};
        for (int depth = 1; depth <= maxDepth && !frontier.isEmpty(); ++depth) {
            QVector<int> next;
            int onLevel = 0;
            for (const int source : frontier) {
                const QVector<int> neighbors = upstream
                    ? document.incoming(source) : document.outgoing(source);
                for (const int candidate : neighbors) {
                    if (candidate < 0 || candidate >= document.sections().size()
                        || seen.contains(candidate)) continue;
                    if (onLevel >= maxPerLevel || nodes.size() >= maxPerSide) {
                        truncated = true;
                        // Limit memory and scene items even for dense cyclic graphs.
                        continue;
                    }
                    seen.insert(candidate);
                    ++onLevel;
                    nodes.append({candidate, depth, source});
                    result.edges.append(upstream
                        ? GraphNeighborhood::Edge{candidate, source}
                        : GraphNeighborhood::Edge{source, candidate});
                    next.append(candidate);
                }
            }
            frontier = std::move(next);
        }
    };
    traverse(true, parentDepth, result.parents, result.parentsTruncated);
    traverse(false, childDepth, result.children, result.childrenTruncated);
    return result;
}
