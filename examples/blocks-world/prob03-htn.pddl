( define ( htn-problem probname )
  ( :domain blocks4 )
  ( :requirements :strips :htn :typing :equality )
  ( :objects
    b070 - block
    b121 - block
    b089 - block
    b010 - block
    b054 - block
    b007 - block
    b029 - block
    b087 - block
    b062 - block
    b122 - block
    b093 - block
    b115 - block
    b006 - block
    b050 - block
    b001 - block
    b004 - block
    b033 - block
    b086 - block
    b000 - block
    b042 - block
    b082 - block
    b017 - block
    b035 - block
    b067 - block
    b024 - block
    b064 - block
    b074 - block
    b020 - block
    b049 - block
    b114 - block
    b012 - block
    b100 - block
    b099 - block
    b066 - block
    b092 - block
    b091 - block
    b075 - block
    b040 - block
    b041 - block
    b124 - block
    b112 - block
    b038 - block
    b036 - block
    b117 - block
    b069 - block
    b085 - block
    b026 - block
    b039 - block
    b079 - block
    b113 - block
    b053 - block
    b068 - block
    b044 - block
    b081 - block
    b063 - block
    b043 - block
    b105 - block
    b102 - block
    b065 - block
    b094 - block
    b107 - block
    b110 - block
    b073 - block
    b109 - block
    b071 - block
    b058 - block
    b032 - block
    b104 - block
    b028 - block
    b045 - block
    b031 - block
    b046 - block
    b051 - block
    b090 - block
    b108 - block
    b056 - block
    b095 - block
    b061 - block
    b059 - block
    b123 - block
    b106 - block
    b083 - block
    b078 - block
    b077 - block
    b097 - block
    b030 - block
    b098 - block
    b103 - block
    b076 - block
    b118 - block
    b101 - block
    b037 - block
    b057 - block
    b096 - block
    b119 - block
    b116 - block
    b088 - block
    b014 - block
    b120 - block
    b018 - block
    b060 - block
    b027 - block
    b022 - block
    b021 - block
    b011 - block
    b015 - block
    b080 - block
    b048 - block
    b016 - block
    b072 - block
    b003 - block
    b002 - block
    b008 - block
    b005 - block
    b019 - block
    b023 - block
    b084 - block
    b009 - block
    b052 - block
    b111 - block
    b034 - block
    b047 - block
    b025 - block
    b013 - block
    b055 - block
  )

  ( :init
    ( on-table b070 )
    ( on b121 b070 )
    ( on b089 b121 )
    ( clear b089 )
    ( on-table b010 )
    ( on b054 b010 )
    ( on b007 b054 )
    ( on b029 b007 )
    ( on b087 b029 )
    ( on b062 b087 )
    ( clear b062 )
    ( on-table b122 )
    ( on b093 b122 )
    ( on b115 b093 )
    ( on b006 b115 )
    ( on b050 b006 )
    ( on b001 b050 )
    ( on b004 b001 )
    ( on b033 b004 )
    ( on b086 b033 )
    ( on b000 b086 )
    ( on b042 b000 )
    ( on b082 b042 )
    ( on b017 b082 )
    ( on b035 b017 )
    ( on b067 b035 )
    ( clear b067 )
    ( on-table b024 )
    ( on b064 b024 )
    ( on b074 b064 )
    ( on b020 b074 )
    ( on b049 b020 )
    ( on b114 b049 )
    ( on b012 b114 )
    ( on b100 b012 )
    ( on b099 b100 )
    ( on b066 b099 )
    ( on b092 b066 )
    ( on b091 b092 )
    ( on b075 b091 )
    ( on b040 b075 )
    ( clear b040 )
    ( on-table b041 )
    ( on b124 b041 )
    ( on b112 b124 )
    ( on b038 b112 )
    ( on b036 b038 )
    ( clear b036 )
    ( on-table b117 )
    ( on b069 b117 )
    ( on b085 b069 )
    ( on b026 b085 )
    ( on b039 b026 )
    ( clear b039 )
    ( on-table b079 )
    ( on b113 b079 )
    ( clear b113 )
    ( on-table b053 )
    ( on b068 b053 )
    ( on b044 b068 )
    ( on b081 b044 )
    ( on b063 b081 )
    ( on b043 b063 )
    ( clear b043 )
    ( on-table b105 )
    ( on b102 b105 )
    ( on b065 b102 )
    ( on b094 b065 )
    ( on b107 b094 )
    ( on b110 b107 )
    ( on b073 b110 )
    ( on b109 b073 )
    ( on b071 b109 )
    ( on b058 b071 )
    ( on b032 b058 )
    ( on b104 b032 )
    ( on b028 b104 )
    ( on b045 b028 )
    ( clear b045 )
    ( on-table b031 )
    ( on b046 b031 )
    ( on b051 b046 )
    ( on b090 b051 )
    ( on b108 b090 )
    ( on b056 b108 )
    ( on b095 b056 )
    ( on b061 b095 )
    ( clear b061 )
    ( on-table b059 )
    ( on b123 b059 )
    ( on b106 b123 )
    ( on b083 b106 )
    ( on b078 b083 )
    ( on b077 b078 )
    ( clear b077 )
    ( on-table b097 )
    ( on b030 b097 )
    ( on b098 b030 )
    ( on b103 b098 )
    ( on b076 b103 )
    ( on b118 b076 )
    ( on b101 b118 )
    ( on b037 b101 )
    ( on b057 b037 )
    ( clear b057 )
    ( on-table b096 )
    ( on b119 b096 )
    ( on b116 b119 )
    ( on b088 b116 )
    ( clear b088 )
    ( on-table b014 )
    ( on b120 b014 )
    ( on b018 b120 )
    ( on b060 b018 )
    ( on b027 b060 )
    ( on b022 b027 )
    ( on b021 b022 )
    ( on b011 b021 )
    ( on b015 b011 )
    ( on b080 b015 )
    ( on b048 b080 )
    ( on b016 b048 )
    ( on b072 b016 )
    ( on b003 b072 )
    ( on b002 b003 )
    ( on b008 b002 )
    ( on b005 b008 )
    ( on b019 b005 )
    ( on b023 b019 )
    ( on b084 b023 )
    ( clear b084 )
    ( on-table b009 )
    ( on b052 b009 )
    ( on b111 b052 )
    ( on b034 b111 )
    ( on b047 b034 )
    ( on b025 b047 )
    ( on b013 b025 )
    ( on b055 b013 )
    ( clear b055 )
    ( hand-empty )
  )
  ( :tasks
    ( make-on-table b068 )
    ( make-on b096 b068 )
    ( make-on b109 b096 )
    ( make-on-table b108 )
    ( make-on b084 b108 )
    ( make-on b113 b084 )
    ( make-on b088 b113 )
    ( make-on b067 b088 )
    ( make-on b044 b067 )
    ( make-on b059 b044 )
    ( make-on b053 b059 )
    ( make-on b047 b053 )
    ( make-on b060 b047 )
    ( make-on-table b097 )
    ( make-on b082 b097 )
    ( make-on b094 b082 )
    ( make-on b098 b094 )
    ( make-on b062 b098 )
    ( make-on b061 b062 )
    ( make-on-table b083 )
    ( make-on b043 b083 )
    ( make-on b055 b043 )
    ( make-on b011 b055 )
    ( make-on b023 b011 )
    ( make-on b101 b023 )
    ( make-on b034 b101 )
    ( make-on b056 b034 )
    ( make-on b019 b056 )
    ( make-on b010 b019 )
    ( make-on b070 b010 )
    ( make-on-table b123 )
    ( make-on b076 b123 )
    ( make-on b035 b076 )
    ( make-on b026 b035 )
    ( make-on b038 b026 )
    ( make-on b079 b038 )
    ( make-on b041 b079 )
    ( make-on b077 b041 )
    ( make-on b112 b077 )
    ( make-on-table b099 )
    ( make-on b022 b099 )
    ( make-on b089 b022 )
    ( make-on b003 b089 )
    ( make-on b050 b003 )
    ( make-on b004 b050 )
    ( make-on b103 b004 )
    ( make-on b058 b103 )
    ( make-on b031 b058 )
    ( make-on b078 b031 )
    ( make-on b045 b078 )
    ( make-on b027 b045 )
    ( make-on b008 b027 )
    ( make-on b036 b008 )
    ( make-on b072 b036 )
    ( make-on-table b086 )
    ( make-on b075 b086 )
    ( make-on b115 b075 )
    ( make-on b080 b115 )
    ( make-on-table b006 )
    ( make-on b114 b006 )
    ( make-on b074 b114 )
    ( make-on b016 b074 )
    ( make-on b046 b016 )
    ( make-on b033 b046 )
    ( make-on b081 b033 )
    ( make-on b020 b081 )
    ( make-on b093 b020 )
    ( make-on b028 b093 )
    ( make-on b087 b028 )
    ( make-on b042 b087 )
    ( make-on b013 b042 )
    ( make-on b030 b013 )
    ( make-on b057 b030 )
    ( make-on b009 b057 )
    ( make-on b007 b009 )
    ( make-on b071 b007 )
    ( make-on-table b106 )
    ( make-on b090 b106 )
    ( make-on b124 b090 )
    ( make-on b107 b124 )
    ( make-on-table b118 )
    ( make-on b021 b118 )
    ( make-on b032 b021 )
    ( make-on b014 b032 )
    ( make-on b024 b014 )
    ( make-on b037 b024 )
    ( make-on b064 b037 )
    ( make-on b017 b064 )
    ( make-on b065 b017 )
    ( make-on b018 b065 )
    ( make-on b104 b018 )
    ( make-on b005 b104 )
    ( make-on b073 b005 )
    ( make-on-table b051 )
    ( make-on b052 b051 )
    ( make-on b066 b052 )
    ( make-on b054 b066 )
    ( make-on b117 b054 )
    ( make-on-table b015 )
    ( make-on b039 b015 )
    ( make-on b040 b039 )
    ( make-on b119 b040 )
    ( make-on b110 b119 )
    ( make-on b029 b110 )
    ( make-on b002 b029 )
    ( make-on b049 b002 )
    ( make-on b116 b049 )
    ( make-on b091 b116 )
    ( make-on b025 b091 )
    ( make-on b001 b025 )
    ( make-on b000 b001 )
    ( make-on b048 b000 )
    ( make-on b069 b048 )
    ( make-on b012 b069 )
    ( make-on b085 b012 )
    ( make-on b063 b085 )
    ( make-on-table b095 )
    ( make-on b092 b095 )
    ( make-on b105 b092 )
    ( make-on b122 b105 )
    ( make-on b102 b122 )
    ( make-on b121 b102 )
    ( make-on b100 b121 )
    ( make-on-table b120 )
    ( make-on b111 b120 )
  )
)

