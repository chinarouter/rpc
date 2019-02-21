STRUCT(s1_req_t)
{
  STRING(name, 32);
  FIELD(uint64 , sweetness);
}__attribute__((packed));

STRUCT(s1_rsp_t)
{
	STRING(name, 32);
	FIELD(char,  dummy);
  FIELD(uint64 , color64);
  FIELD(uint32 , color32);
}__attribute__((packed));


STRUCT(s2_req_t)
{
	STRING(name, 32);
	ARRAY(s1_req_t, s1_req, 4);
	FIELD(int, c);
};
