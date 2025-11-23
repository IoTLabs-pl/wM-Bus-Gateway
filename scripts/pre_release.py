#!/usr/bin/env python3

import argparse
from logging import getLogger

from release_utils import (
    COMMON_PARSER,
    PARENT_DIR,
    CommonArgs,
    Tag,
    checkout_master,
    ensure_clean_tree,
    git,
    replace_versions_content,
    setup_logging,
)

logger = getLogger(__name__)


def get_latest_tag():
    output = git("describe", "--tags", "--abbrev=0", "--match", "v*", dry_run=False)
    return Tag(output.strip())


def parse_args():
    class Args(CommonArgs):
        bump: str | None = None
        version: Tag | None = None

    parser = argparse.ArgumentParser(
        description="Tag a new version.",
        parents=[COMMON_PARSER],
    )

    group = parser.add_mutually_exclusive_group(required=True)
    group.add_argument(
        "--bump",
        choices=["major", "minor", "patch"],
        help="Type of version bump (major, minor, patch).",
    )
    group.add_argument(
        "--version",
        help="Specify an explicit version (e.g., 1.2.3).",
        type=Tag,
    )

    return parser.parse_args(namespace=Args())


if __name__ == "__main__":
    args = parse_args()

    setup_logging(args)

    checkout_master(args.dry_run)
    ensure_clean_tree(args.dry_run)

    latest_tag = get_latest_tag()

    if args.version:
        new_tag = args.version
    else:
        new_tag = Tag(str(getattr(latest_tag, f"next_{args.bump}")()))

    input(f"Press Enter to continue with version {new_tag} (or Ctrl+C to cancel)...")

    replace_versions_content(
        latest_tag,
        new_tag,
        {
            PARENT_DIR
            / "packages"
            / "version.yaml": "wmbus_gateway_version: {version}",
            PARENT_DIR / "blueprint.yaml": "wmbus_gateway.yaml@{version}",
        },
        dry_run=args.dry_run,
    )

    logger.info("Committing version update...")
    git("commit", "-am", f"Release {new_tag}", dry_run=args.dry_run)

    for tag in [new_tag, "latest"]:
        logger.info(f"Tagging with: {tag}")
        git("tag", "--force", tag, dry_run=args.dry_run)

    logger.info("Pushing changes...")
    git("push", dry_run=args.dry_run)
    git("push", "--tags", "--force", dry_run=args.dry_run)
