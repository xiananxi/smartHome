#include <stdio.h>
#include "control.h"

struct control *add_interface_to_ctrl_list(struct control *head,struct control *interface )
{
    if(head == NULL){
        head = interface;

        return head;
    }else{
        interface->next = head;
        head = interface;

        return head;
    }

}
