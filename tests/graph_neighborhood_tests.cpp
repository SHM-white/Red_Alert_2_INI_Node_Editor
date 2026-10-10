#include "../modern/GraphNeighborhood.h"
#include <QCoreApplication>
#include <QSet>
#include <QDebug>

static void require(bool passed, const char* message) {
    if (!passed) qFatal("Graph test failed: %s", message);
}
static bool has(const QVector<GraphNeighborhood::Node>& nodes, int section, int depth) {
    for (const auto& n : nodes)
        if (n.section == section && n.depth == depth) return true;
    return false;
}
static bool edge(const GraphNeighborhood& graph, int from, int to) {
    for (const auto& e : graph.edges)
        if (e.from == from && e.to == to) return true;
    return false;
}
int main(int argc, char** argv) {
    QCoreApplication app(argc, argv);
    IniDocument doc;
    doc.setLines({"[A]", "next=B", "[B]", "next=C", "[C]", "next=D", "[D]", "next=E",
                  "[E]", "next=F", "[F]", "next=B", "[Unrelated]", "x=unknown"});
    const int a = doc.indexOf("A"), b = doc.indexOf("B"), c = doc.indexOf("C");
    const int d = doc.indexOf("D"), e = doc.indexOf("E"), f = doc.indexOf("F");
    const auto old = buildNeighborhood(doc, c, 1, 1);
    require(old.parents.size() == 1 && has(old.parents, b, 1), "default parent depth 1");
    require(old.children.size() == 1 && has(old.children, d, 1), "default child depth 1");
    require(edge(old, b, c) && edge(old, c, d), "direct edges");

    const auto deep = buildNeighborhood(doc, c, 3, 3);
    require(has(deep.parents, b, 1) && has(deep.parents, a, 2), "two parents");
    require(has(deep.children, d, 1) && has(deep.children, e, 2)
            && has(deep.children, f, 3), "three children");
    require(edge(deep, a, b) && edge(deep, d, e) && edge(deep, e, f), "deep oriented edges");
    QSet<int> ids{c};
    for (const auto& n : deep.parents) { require(!ids.contains(n.section), "no duplicate parent"); ids.insert(n.section); }
    for (const auto& n : deep.children) { require(!ids.contains(n.section), "no duplicate child"); ids.insert(n.section); }

    const auto noParents = buildNeighborhood(doc, c, 0, 4);
    require(noParents.parents.isEmpty(), "zero parent depth hides parents");
    require(has(noParents.children, f, 3), "child depth independent");
    const auto noChildren = buildNeighborhood(doc, c, 2, 0);
    require(noChildren.children.isEmpty(), "zero child depth hides children");
    require(has(noChildren.parents, a, 2), "parent depth independent");

    IniDocument fan;
    fan.setLines({"[Hub]", "links=N1,N2,N3,N4,N5,N6,N7,N8,N9,N10,N11,N12,N13,N14,N15",
                  "[N1]", "[N2]", "[N3]", "[N4]", "[N5]", "[N6]", "[N7]",
                  "[N8]", "[N9]", "[N10]", "[N11]", "[N12]", "[N13]", "[N14]", "[N15]"});
    const auto capped = buildNeighborhood(fan, fan.indexOf("Hub"), 0, 1, 14, 64);
    require(capped.children.size() == 14 && capped.childrenTruncated, "breadth cap and overflow");
    const auto total = buildNeighborhood(fan, fan.indexOf("Hub"), 0, 1, 50, 5);
    require(total.children.size() == 5 && total.childrenTruncated, "side cap");
    const auto off = buildNeighborhood(doc, c, -100, 100);
    require(off.parents.isEmpty() && off.children.size() <= 5, "clamp inputs");
    require(buildNeighborhood(doc, -1, 2, 2).edges.isEmpty(), "invalid selection safe");
    qInfo() << "Graph neighborhood tests passed";
}
