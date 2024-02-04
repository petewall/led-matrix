def kv: to_entries[] | "\(.key)=\(.value)";

. | to_entries[]
| "[\(.key)]", (.value|kv)
