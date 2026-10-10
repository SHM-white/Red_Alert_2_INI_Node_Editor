#pragma once

#include "IniDocument.h"
#include <QVector>

// The graph model is independent of the QGraphicsScene and can be tested headlessly.
// Each item is one unique section at the shortest discovered distance from the focus.
struct GraphNeighborhood {
    struct Node {
        int section = -1;
        int depth = 0;
        int nextToFocus = -1;
    };
    struct Edge {
        int from = -1;
        int to = -1;
    };
    QVector<Node> parents;
    QVector<Node> children;
    QVector<Edge> edges;
    bool parentsTruncated = false;
    bool childrenTruncated = false;
};

// 0 means hide that direction. Traversal is breadth-first, bounded, and cycle-safe.
// Parents are visited first; a cycle/section appearing on both sides is only drawn once.
GraphNeighborhood buildNeighborhood(const IniDocument& document, int focus,
                                     int parentDepth, int childDepth,
                                     int maxPerLevel = 14, int maxPerSide = 64);
