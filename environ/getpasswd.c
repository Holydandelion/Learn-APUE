#include <pwd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <shadow.h>
#include <errno.h>

struct passwd * get_uid_by_name(const char *  name);

struct passwd * get_uid_by_name(const char *  name)
{
	struct passwd *passwd_ptr = NULL;
	setpwent();
	while ( (passwd_ptr = getpwent()) != NULL)
	{
		if (strcmp(name, passwd_ptr->pw_name) == 0)
			break;
	}
	endpwent();
	return passwd_ptr;
}

int main(void)
{
	struct passwd *pwd_ptr = NULL;
	struct spwd *spwd_ptr = NULL;
	
	pwd_ptr = get_uid_by_name("chenruogeng");
	printf("[DEBUG] uid:%d \n", pwd_ptr->pw_uid);
	printf("[DEBUG] gid:%d \n", pwd_ptr->pw_gid);	
	printf("[DEBUG] password:%s \n", pwd_ptr->pw_passwd);
	printf("[DEBUG] dir:%s \n", pwd_ptr->pw_dir);
	printf("[DEBUG] shell:%s \n", pwd_ptr->pw_shell);			
	
	if ((spwd_ptr = getspnam("chenruogeng")) != NULL)
		printf("[DEBUG]shadow password:%s \n", spwd_ptr->sp_pwdp);
	else
		printf("[DEBUG] getspnam error, %s \n", strerror(errno));
	
	exit(0);
}


