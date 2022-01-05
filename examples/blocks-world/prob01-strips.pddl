( define ( problem probname )
  ( :domain blocks4 )
  ( :requirements :strips :typing :equality )

  ( :objects
    b001 - block
    b000 - block
    b003 - block
    b002 - block
    b004 - block
  )

  ( :init
    ( on-table b001 )
    ( on b000 b001 )
    ( on b003 b000 )
    ( clear b003 )
    ( on-table b002 )
    ( on b004 b002 )
    ( clear b004 )
    ( hand-empty )
  )
  ( :goal
    ( and
      ( on-table b000 )
      ( on b004 b000 )
      ( clear b004 )
      ( on-table b002 )
      ( on b003 b002 )
      ( clear b003 )
      ( on-table b001 )
      ( clear b001 )
    )
  )
)

