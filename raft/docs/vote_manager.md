
Reset Election

```mermaid
sequenceDiagram
    vote_manager ->> vote_manager: timeout and reset election
    vote_manager ->> vote_manager: reset all states except term
```


```mermaid
sequenceDiagram
    leader ->> vote_manager: any msg
    vote_manager ->> vote_manager: reset randomize timer for election reset
```