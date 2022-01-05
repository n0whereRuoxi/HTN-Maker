( define
  ( tasks Rovers-tasks )

  ( :task Get_Soil_Data
    :parameters
    (
      ?location - waypoint
    )
    :precondition
    (
    )
    :effect
    ( and
      ( communicated_soil_data ?location )
    )
  )

  ( :task Get_Rock_Data
    :parameters
    (
      ?location - waypoint
    )
    :precondition
    (
    )
    :effect
    ( and
      ( communicated_rock_data ?location )
    )
  )

  ( :task Get_Image_Data
    :parameters
    (
      ?objective - objective
      ?mode - mode
    )
    :precondition
    (
    )
    :effect
    ( and
      ( communicated_image_data ?objective ?mode )
    )
  )

)