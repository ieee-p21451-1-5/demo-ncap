#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>

#include <stdio.h>
#include <stdlib.h>

#include "ieeeP1451Utils.h"

/* returns a float value ranging from 0. to 1. */
float	randf()
{
	return ((float) rand()) / ((float) RAND_MAX);
}

/**** Tool Functions for Understanding Net-SNMP Data Structures ****/

void	print_oid(oid *name, size_t name_length)
{
	/* XXX: the variable type & size might be buggy */
	int i;
	printf("%u", name[0]);
	for (i = 1; i < name_length; i++) {
		printf(".%u", name[i]);
	}
}

void	print_printable(unsigned char c)
{
	if (c >= 0x20 && c <= 0x7e) {
		printf("%c", c);
	} else {
		switch (c) {
		case 0x00:
			printf("\\0");
			break;
		case 0x09:
			printf("\\t");
			break;
		case 0x0a:
			printf("\\n");
			break;
		case 0x0d:
			printf("\\r");
			break;
		default:
			printf(".");
		}
	}
}

void	print_var_binding_s_value_part(netsnmp_variable_list *requestvb)
{
	netsnmp_vardata	val		= requestvb->val;
	u_char		type		= requestvb->type;
	size_t		val_len		= requestvb->val_len;
	u_char		*buf		= requestvb->buf;
	int		i;
	int		newline_cnt;

	/* TODO: complete all possible types */
	switch (type) {
	case 2U:
		/* snmpset with `i' type indicator */
		printf("%d", *(val.integer));
		break;

	case 4U:
		/* snmpset with `s/d/x' type indicator */
		// printf("%s", val.string);
		for (i = 0; i < val_len; i++) {
			print_printable(val.string[i]);
		}

		printf("\n  ");

		/* see what buf[] holds */
		newline_cnt = 0;
		for (i = 0; i < val_len; i++) {
			printf("%02x ", buf[i]);
			if (++newline_cnt >= BYTE_NUM_PER_LINE) {
				newline_cnt = 0;
				printf("\n  ");
			}
		}
		break;
	case 6U:
		/* snmpset with `o' type indicator */
		/* XXX: the variable type & size might be buggy */
		print_oid(val.objid, val_len/sizeof(oid)); 
		break;
	case 64U:
		/* snmpset with `a' type indicator */
		printf("%u.%u.%u.%u", val.bitstring[0],
		                      val.bitstring[1],
		                      val.bitstring[2],
		                      val.bitstring[3]);
		break;
	case 66U:
		/* snmpset with `u' type indicator */
		/* XXX: the variable type & size might be buggy */ 
		/* though tested OK: 0 ~ 4294967295 */
		printf("%d", *((unsigned long *) val.integer));
		break;
	case 67U:
		/* snmpset with `t' type indicator */
		/* TODO */
		break;
	default:
		break;
	}
}

void	request_dump(netsnmp_request_info *requests)
{
	/* XXX: the variable type & size might be buggy */
	/*
	  some typedefs on dell Ubuntu (not on Pi)
	  oid		u_long
	  name_length	size_t (always)
	  type		u_char (always)
	*/	
	printf("────────────────SNMP request dump────────────────\n");
	printf("name_len:       %u\n", requests->requestvb->name_length);
	// printf("	name:		");
	// print_objid(requests->requestvb->name, requests->requestvb->name_length);
	printf("name:           ");
	print_oid(requests->requestvb->name, requests->requestvb->name_length);
	printf("\n");
	printf("type:           %u\n", requests->requestvb->type);
	printf("val_len:        %u\n", requests->requestvb->val_len);
	printf("value:          ");
	print_var_binding_s_value_part(requests->requestvb);
	// print_value()
	printf("\n");

	printf("─────────────────────────────────────────────────\n");
}