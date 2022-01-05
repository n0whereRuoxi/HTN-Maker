( define ( htn-problem probname )
  ( :domain blocks4 )
  ( :requirements :strips :htn :typing :equality )
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
  ( :tasks
    ( make-on-table b000 )
    ( make-on b004 b000 )
    ( make-on-table b002 )
    ( make-on b003 b002 )
    ( make-on-table b001 )
  )
)

