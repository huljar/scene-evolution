# Scene Evolution

`scene-evolution` is a graphical tool that allows a user to load RGB/D scenes
(i.e., images with depth information for each pixel) into a navigatable 3D view
that can be freely navigated. These scenes can then be modified based on their
labeling (which is contained in a corresponding label file). Using an SQL-like
scripting syntax called *SEL* (**S**cene **E**volution **L**anguage), objects
(or collections of objects) in the scene may be selected, which can then be
moved around, rotated, and deleted.

## Dataset

This tool is designed to work with the NYU Depth datasets, which can be obtained
[here (v1)](https://cs.nyu.edu/~silberman/datasets/nyu_depth_v1.html) and [here
(v2)](https://cs.nyu.edu/~silberman/datasets/nyu_depth_v2.html). A few example
scenes are included in `resources/scenes`.

When running the tool, it will first ask for the location of a dataset. There,
specify the base directory of the dataset (the one containing the `depth`,
`label`, and `rgb` subdirectories).

## SEL

*coming soon*

## License

This software is licensed under the terms of the GNU General Public License
version 3 (except third party components). For more information, please refer to
the [LICENSE.md](LICENSE.md) and
[LICENSES-THIRD-PARTY.md](LICENSES-THIRD-PARTY.md) files.
