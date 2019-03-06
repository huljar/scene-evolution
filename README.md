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

SEL is the language used for manipulating objects and scenes. Its grammar is
given below.

<details><summary>Grammar of the SEL language</summary>
<p>

```
query_list:
    query 
  | query_list query

query:
    select_statement ";" action_list ";"

select_statement:
    "select" object_list optional_from_statement optional_where_statement 

optional_from_statement:
    
  | "from scene" "integer"

optional_where_statement:
    
  | "where" search_condition

object_list:
    object 
  | object_list "," object

object:
    qualifier_list identifier_concat

identifier_concat:
    "identifier" 
  | identifier_concat "identifier" 

qualifier_list:
    
  | qualifier_list qualifier

qualifier:
    "qualifier"

search_condition:
    boolean_term 
  | search_condition "or" boolean_term

boolean_term:
    boolean_factor 
  | boolean_term "and" boolean_factor

boolean_factor:
    boolean_test 
  | "not" boolean_test

boolean_test:
    predicate 
  | "(" search_condition ")"

predicate:
    comp_predicate 
  | binary_predicate

comp_predicate:
    comp_element "=" comp_element 
  | comp_element "!=" comp_element 
  | comp_element "<" comp_element 
  | comp_element ">" comp_element 
  | comp_element "<=" comp_element 
  | comp_element ">=" comp_element

comp_element:
    numeric_value 
  | distance_term

distance_term:
    "distance to" object

binary_predicate:
    boolean_value 
  | support_term

support_term:
    "supported by" object

numeric_value:
    "integer" 
  | "float"

boolean_value:
    "boolean"

action_list:
    action 
  | action_list ";" action

action:
    move_action 
  | remove_action 
  | rotate_action 
  | copy_action

move_action:
    "move to" object 
  | "move to" object "where" search_condition 
  | "move by" vector

remove_action:
    "remove"

rotate_action:
    "rotate by" vector

copy_action:
    "copy at" object 
  | "copy at" object "where" search_condition

vector:
    "(" numeric_value "," numeric_value "," numeric_value ")"
```

</p>
</details>

## License

This software is licensed under the terms of the GNU General Public License
version 3 (except third party components). For more information, please refer to
the [LICENSE.md](LICENSE.md) and
[LICENSES-THIRD-PARTY.md](LICENSES-THIRD-PARTY.md) files.
