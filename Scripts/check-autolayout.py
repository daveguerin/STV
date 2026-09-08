#!/usr/bin/env python3
"""Guard STV's constraint-layout and app-owned-container boundaries.

UIKit frame reads for drawing, caret scrolling, popover anchors, and Interface
Builder estimates are allowed. Explicit frame/center/bounds writes, STV manual
text measurement, content autoresizing masks, and split ownership are not.
The exact UIKit input-root flexible-height handshake is an explicit exception.
Runtime XCTest coverage remains necessary to verify the constraints themselves.
"""
from pathlib import Path
import re
import sys

ROOT = Path(__file__).resolve().parents[1]
PATTERNS = {
    "manual view geometry": re.compile(
        r"\.(?:frame|bounds|center)(?:\.\w+)*\s*=(?!=)|"
        r"\bset(?:Frame|Bounds|Center)\s*:"
    ),
    "manual content measurement": re.compile(
        r"\b(?:sizeToFit|sizeThatFits|boundingRectWithSize|sizeWithAttributes|"
        r"usedRectForTextContainer|ensureLayoutForTextContainer)\b"
    ),
    "autoresizing-mask layout": re.compile(r"\.autoresizingMask\s*="),
    "split-view integration": re.compile(r"\b\w*(?:SplitView|splitView)\w*\b"),
}


def violations(root):
    findings = []
    for path in sorted((root / "Sources").glob("*")):
        if path.suffix not in {".h", ".m"}:
            continue
        for number, line in enumerate(path.read_text().splitlines(), 1):
            for reason, pattern in PATTERNS.items():
                if pattern.search(line):
                    # Native keyboard-container handshake, not content layout.
                    # Verified with a launched iPad app: without it root height
                    # is zero even though Auto Layout fitting returns 44.
                    if (reason == "autoresizing-mask layout"
                            and path.name == "SCInputAccessoryView.m"
                            and line.strip() ==
                            "self.autoresizingMask = UIViewAutoresizingFlexibleHeight;"):
                        continue
                    findings.append((str(path.relative_to(root)), number,
                                     reason, line.strip()))
    return findings


def main():
    findings = violations(ROOT)
    for path, number, reason, line in findings:
        print(f"{path}:{number}: {reason}: {line}")
    if findings:
        print(f"FAIL: {len(findings)} legacy layout references")
        return 1
    print("PASS: no STV-owned frame layout, manual text measurement, "
          "content autoresizing-mask layout, or split-view integration in Sources; "
          "native keyboard-root flexible-height handshake allowed")
    return 0


if __name__ == "__main__":
    sys.exit(main())
