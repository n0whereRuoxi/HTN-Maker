( define 
  ( tasks Blocks4-tasks )

  ( :task Make-Clear
    :parameters
    (
      ?theblock - block
    )
    :precondition
    (
    )
    :effect
    ( and
      ( clear ?theblock)
    )
  )

)

