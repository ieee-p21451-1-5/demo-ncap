#define		BYTE_NUM_PER_LINE	16

float	randf();

/**** Tool Functions for Understanding Net-SNMP Data Structures ****/

void	print_oid(oid *name, size_t name_length);
void	print_printable(unsigned char c);
void	print_var_binding_s_value_part(netsnmp_variable_list *requestvb);
void	request_dump(netsnmp_request_info *requests);