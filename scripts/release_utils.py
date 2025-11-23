import argparse
import difflib
import logging
from pathlib import Path

from esphome.git import run_git_command
from semantic_version import Version

logger = logging.getLogger(__name__)


PARENT_DIR = Path(__file__).parent.parent.resolve()


COMMON_PARSER = argparse.ArgumentParser(add_help=False)
COMMON_PARSER.add_argument(
    "--dry-run",
    action="store_true",
    help="Show what would be done, but don't make any changes.",
)
COMMON_PARSER.add_argument(
    "-v",
    "--verbose",
    action="store_true",
    help="Enable verbose (DEBUG) logging.",
)


class CommonArgs(argparse.Namespace):
    dry_run: bool
    verbose: bool


def setup_logging(args: CommonArgs):
    logging.basicConfig(
        level=logging.DEBUG if args.verbose else logging.INFO,
    )


class Tag(Version):
    TAG_PREFIX = "v"

    def __init__(self, string_version: str):
        super().__init__(string_version.removeprefix(self.TAG_PREFIX))

    def __str__(self):
        return f"{self.TAG_PREFIX}{super().__str__()}"


def git(*args, dry_run=True):
    args = tuple(map(str, args))
    if dry_run:
        logger.info(f"Would run: git {' '.join(args)}")
        return ""
    return run_git_command(["git", *args], git_dir=PARENT_DIR)


def checkout_master(dry_run=True):
    git("checkout", "master", dry_run=dry_run)


def ensure_clean_tree(dry_run=True):
    output = git("status", "--porcelain", dry_run=dry_run)
    if output and not dry_run:
        raise RuntimeError(
            "Git tree is not clean. Please commit or stash your changes before tagging."
        )


def replace_versions_content(
    old_version: Tag,
    new_version: Tag,
    replacements: dict[Path, str],
    dry_run=True,
):
    for file, replacement in replacements.items():
        content = file.read_text()
        new_content = content.replace(
            replacement.format(version=old_version),
            replacement.format(version=new_version),
        )
        if content != new_content:
            diff = difflib.unified_diff(
                content.splitlines(),
                new_content.splitlines(),
                lineterm="",
            )
            logger.info(
                f"Updating {file.relative_to(PARENT_DIR)} from {old_version} to {new_version}"
            )
            for line in diff:
                logger.debug(line)
            if not dry_run:
                file.write_text(new_content)
        else:
            raise RuntimeError(f"No changes made to {file}")


def common_parser():
    return COMMON_PARSER
