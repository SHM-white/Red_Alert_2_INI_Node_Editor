#include "IniDocument.h"
#include <cassert>
#include <iostream>

int main() {
    IniDocument doc;
    const std::string original = "\xEF\xBB\xBF; header\r\n[InfantryTypes] ; list\r\n1=E1\r\n\r\n[E1]\r\nName=GI  ; preserved\r\nPrimary=M60\r\nName2=foo;bar\r\n";
    doc.parse(original);
    assert(doc.serialize() == original);
    assert(doc.utf8());
    assert(doc.sections().size() == 2);
    assert(doc.sections()[1].entries[0].value == "GI");
    assert(doc.sections()[1].entries[2].value == "foo;bar");
    assert(doc.setValue(1, 0, "Guardian"));
    assert(doc.serialize().find("Name=Guardian  ; preserved") != std::string::npos);
    assert(doc.renameKey(1, 1, "Secondary"));
    assert(doc.addKey(1, "Strength", "125"));
    assert(!doc.addKey(1, "strength", "10"));
    assert(doc.renameSection(1, "E2"));
    assert(!doc.addSection("e2"));
    assert(doc.addSection("Vehicle"));
    assert(doc.removeKey(1, 0));
    assert(doc.removeSection(2));
    const auto changed = doc.serialize();
    IniDocument reread; reread.parse(changed);
    assert(reread.serialize() == changed);
    assert(reread.findSection("e2") == 1);

    IniDocument noEol;
    noEol.parse("[A]\nK=V");
    assert(noEol.serialize() == "[A]\nK=V");
    assert(noEol.addKey(0, "N", "M"));
    assert(noEol.serialize() == "[A]\nK=V\nN=M");

    IniDocument legacy;
    legacy.parse(std::string("[Unit]\r\nName=\xD6\xD0\xCE\xC4\r\n"));
    assert(!legacy.utf8());
    const auto bytes = legacy.serialize();
    assert(legacy.setValue(0, 0, std::string("\xD6\xD0\xCE\xC4")));
    assert(legacy.serialize() == bytes);

    IniDocument cycle;
    cycle.parse("[A]\r[B]\nName=Me");
    assert(cycle.sections().size() == 2);
    assert(cycle.serialize() == "[A]\r[B]\nName=Me");

    std::cout << "INI roundtrip/edit/legacy tests passed\n";
}
