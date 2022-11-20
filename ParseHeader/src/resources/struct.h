#ifndef __STRUCT_H__
#define __STRUCT_H__

// Phone
typedef struct Phone
{
	int m_type;
} Phone;

// People
typedef struct People
{
	char    m_sex;			/* 性别 */
	int     m_age;			/* 
	
	年龄
	
	*/
	char *  m_name;			// 名称
	int     m_phone_num;
	Phone * m_phone;
} People;

#endif

