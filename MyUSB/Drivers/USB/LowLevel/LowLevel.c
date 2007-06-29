#include "LowLevel.h"

U8 usb_config_ep(U8 config0, U8 config1)
{
    Usb_enable_endpoint();
    UECFG0X = config0;
    UECFG1X = (UECFG1X & (1<<ALLOC)) | config1;
    Usb_allocate_memory();
    return (Is_endpoint_configured());
}

U8 usb_select_enpoint_interrupt(void)
{
U8 interrupt_flags;
U8 ep_num;

   ep_num = 0;
   interrupt_flags = Usb_interrupt_flags();

   while(ep_num < MAX_EP_NB)
   {
      if (interrupt_flags & 1)
      {
         return (ep_num);
      }
      else
      {
         ep_num++;
         interrupt_flags = interrupt_flags >> 1;
      }
   }
   return 0;
}

U8 usb_send_packet(U8 ep_num, U8* tbuf, U8 data_length)
{
U8 remaining_length;

   remaining_length = data_length;
   Usb_select_endpoint(ep_num);
   while(Is_usb_write_enabled() && (0 != remaining_length))
   {
      Usb_write_byte(*tbuf);
      remaining_length--;
      tbuf++;
   }
   return remaining_length;
}

U8 usb_read_packet(U8 ep_num, U8* rbuf, U8  data_length)
{
U8 remaining_length;

   remaining_length = data_length;
   Usb_select_endpoint(ep_num);

   while(Is_usb_read_enabled() && (0 != remaining_length))
   {
      *rbuf = Usb_read_byte();
      remaining_length--;
      rbuf++;
   }
   return remaining_length;
}

void usb_halt_endpoint (U8 ep_num)
{
   Usb_select_endpoint(ep_num);
   Usb_enable_stall_handshake();
}

U8 usb_init_device (void)
{
   Usb_select_device();
   if(Is_usb_id_device())
   {
      Usb_select_endpoint(EP_CONTROL);
      if(!Is_usb_endpoint_enabled())
      {
         return usb_configure_endpoint(EP_CONTROL,    \
                                TYPE_CONTROL,  \
                                DIRECTION_OUT, \
                                SIZE_64,       \
                                ONE_BANK,      \
                                NYET_DISABLED);
      }
   }
   return FALSE;
}