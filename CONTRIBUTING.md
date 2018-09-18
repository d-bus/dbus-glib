# Contributing to dbus-glib

The short version is "don't". Please contribute to (and use) a better
project instead, for example GDBus in GLib's GIO module.

However, if you are really sure that dbus-glib is the project to which
you want to be contributing, read on.

## Source code repository and issue tracking

dbus-glib is hosted by freedesktop.org. The source code repository,
issue tracking and merge requests are provided by freedesktop.org's
Gitlab installation: <https://gitlab.freedesktop.org/dbus/dbus-glib>

## Making changes

If you are making changes that you wish to be incorporated upstream,
please do as small commits to your local git tree that are individually
correct, so there is a good history of your changes.

The first line of the commit message should be a single sentence that
describes the change, optionally with a prefix that identifies the
area of the code that is affected.

The body of the commit message should describe what the patch changes
and why, and also note any particular side effects. This shouldn't be
empty on most of the cases. It shouldn't take a lot of effort to write a
commit message for an obvious change, so an empty commit message body is
only acceptable if the questions "What?" and "Why?" are already answered
on the one-line summary.

The lines of the commit message should have at most 76 characters,
to cope with the way git log presents them.

See [notes on commit messages](https://who-t.blogspot.com/2009/12/on-commit-messages.html),
[A Note About Git Commit Messages](https://tbaggery.com/2008/04/19/a-note-about-git-commit-messages.html)
or [How to Write a Git Commit Message](https://chris.beams.io/posts/git-commit/)
for recommended reading on writing high-quality commit messages.

Your patches should also include a Signed-off-by line with your name and
email address, indicating that your contribution follows the [Developer's
Certificate of Origin](https://developercertificate.org/). If you're
not the patch's original author, you should also gather S-o-b's by
them (and/or whomever gave the patch to you.) The significance of this
is that it certifies that you created the patch, that it was created
under an appropriate open source license, or provided to you under those
terms. This lets us indicate a chain of responsibility for the copyright
status of the code.

We won't reject patches that lack S-o-b, but it is strongly recommended.

When you consider changes ready for merging to mainline:

* create a personal fork of <https://gitlab.freedesktop.org/dbus/dbus-glib>
  on freedesktop.org Gitlab
* push your changes to your personal fork as a branch
* create a merge request at
  <https://gitlab.freedesktop.org/dbus/dbus-glib/merge_requests>

## Automated tests

For nontrivial changes please try to extend the test suite to cover it.  The
test infrastructure is in `test/core/`. If possible, use a GTester-style test
like `registrations.c`; or if necessary, add things to the giant catch-all
test, `test-dbus-glib.c`.

Run `make check` to run the test suite.

## Coding style

Please match the existing code style (Emacs: "gnu").

## Licensing

Please match the existing licensing (a dual-license: AFL-2.1 or GPL-2+,
recipient's choice). Entirely new modules can be placed under a more
permissive license: to avoid license proliferation, our preferred
permissive license is the variant of the MIT/X11 license used by the
Expat XML library (for example see the top of tools/ci-build.sh).

## Conduct

As a freedesktop.org project, dbus follows the Contributor Covenant,
found at: <https://www.freedesktop.org/wiki/CodeOfConduct>

Please conduct yourself in a respectful and civilised manner when
interacting with community members on mailing lists, IRC, or bug
trackers. The community represents the project as a whole, and abusive
or bullying behaviour is not tolerated by the project.

## Versioning

Version 0.N, where *N* is even (divisible by 2), is a real release.

Point releases can be versioned 0.N.M where *N* is even and *M* is
any number greater than zero.

Version 0.(N+1), where *N* is even (divisible by 2), identifies a
development snapshot leading to version 0.(N+2). Odd-numbered versions
should never be used as releases.

## Information for maintainers

This section is not directly relevant to infrequent contributors.

### dbus-gmain

dbus-gmain is maintained via `git subtree`. To update, assuming you have
a checkout of the dbus-gmain branch of the dbus-glib repository in
../dbus-gmain:

    git subtree pull -P dbus-gmain ../dbus-gmain HEAD

### Committing other people's patches

If applying a patch from someone else that created them via
"git-format-patch", you can use "git-am -s" to apply.  Otherwise
apply the patch and then use "git commit --author ..."

Nontrivial patches should always go through Gitlab for peer review,
so you should have an issue number or a merge request ID to refer to.

### Making a release

dbus-glib uses an even-stable odd-development release numbering
system. The current number in configure.ac should be odd, except
when releasing.

To make a release (please replace use of 0.76 with the new version)

* make
* make distcheck
* edit configure.ac, change version to even (e.g. 0.75 -> 0.76)
* also in configure.ac, update `LT_CURRENT`, `LT_REVISION` and `LT_AGE`
* edit NEWS, summarize notable changes since the last release
* `./autogen.sh`
* `make`
* `make distcheck`
* sign the tarball (or use: `make dbus-glib-0.76.tar.gz.asc`)
* `make maintainer-upload-release`
* `git commit -m "Release"`
* `git tag -a dbus-glib_0.76`
* edit configure.ac, change version to odd (e.g. 0.76 -> 0.77)
* `git commit -m "Bump version for development"`
* Update the wiki: http://www.freedesktop.org/wiki/Software/DBusBindings
* Announce the release on the mailing list, quoting the notable changes
  from NEWS.
