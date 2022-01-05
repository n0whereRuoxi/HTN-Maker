( define ( problem probname )
  ( :domain blocks4 )
  ( :requirements :strips :typing :equality )

  ( :objects
    b001 - block
    b003 - block
    b002 - block
    b024 - block
    b008 - block
    b020 - block
    b017 - block
    b015 - block
    b012 - block
    b019 - block
    b013 - block
    b021 - block
    b007 - block
    b000 - block
    b011 - block
    b005 - block
    b018 - block
    b006 - block
    b010 - block
    b004 - block
    b009 - block
    b016 - block
    b022 - block
    b014 - block
    b023 - block
  )

  ( :init
    ( on-table b001 )
    ( on b003 b001 )
    ( on b002 b003 )
    ( on b024 b002 )
    ( on b008 b024 )
    ( on b020 b008 )
    ( clear b020 )
    ( on-table b017 )
    ( on b015 b017 )
    ( on b012 b015 )
    ( on b019 b012 )
    ( on b013 b019 )
    ( on b021 b013 )
    ( clear b021 )
    ( on-table b007 )
    ( on b000 b007 )
    ( on b011 b000 )
    ( on b005 b011 )
    ( on b018 b005 )
    ( on b006 b018 )
    ( on b010 b006 )
    ( on b004 b010 )
    ( on b009 b004 )
    ( on b016 b009 )
    ( clear b016 )
    ( on-table b022 )
    ( on b014 b022 )
    ( on b023 b014 )
    ( clear b023 )
    ( hand-empty )
  )
  ( :goal
    ( and
      ( on-table b020 )
      ( on b006 b020 )
      ( on b008 b006 )
      ( on b003 b008 )
      ( on b007 b003 )
      ( on b019 b007 )
      ( clear b019 )
      ( on-table b013 )
      ( on b012 b013 )
      ( clear b012 )
      ( on-table b016 )
      ( on b011 b016 )
      ( on b004 b011 )
      ( on b005 b004 )
      ( clear b005 )
      ( on-table b017 )
      ( on b010 b017 )
      ( clear b010 )
      ( on-table b002 )
      ( on b009 b002 )
      ( on b021 b009 )
      ( on b014 b021 )
      ( clear b014 )
      ( on-table b023 )
      ( on b022 b023 )
      ( on b001 b022 )
      ( on b024 b001 )
      ( on b000 b024 )
      ( clear b000 )
      ( on-table b015 )
      ( on b018 b015 )
      ( clear b018 )
    )
  )
)

