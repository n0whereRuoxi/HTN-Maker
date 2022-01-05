( define
  ( tasks Depots-tasks )

  ( :task Make-On
    :parameters
    (
      ?above - surface
      ?below - surface
    )
    :precondition
    (
    )
    :effect
    ( and
      ( on ?above ?below )
    )
  )

)
