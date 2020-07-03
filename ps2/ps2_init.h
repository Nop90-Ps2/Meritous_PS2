#include <stdio.h>
#include <stdlib.h>
#include <tamtypes.h>
#include <sifrpc.h>
#include <kernel.h>
#include <loadfile.h>
#include <errno.h>
#include <iopheap.h>
#include <debug.h>
#include <iopcontrol.h>
#include <sbv_patches.h>
#include <libmc.h>


int CreateSave(void);

void PS2_init() {
    #ifndef PS2LINK
    SifIopReset(NULL, 0); // clean previous loading of irx by apps like ulaunchElf. Comment this line to get cout on ps2link
    #endif

// change priority to make SDL audio thread run properly
    int main_id = GetThreadId();
    ChangeThreadPriority(main_id, 72);

    // Initialize and connect to all SIF services on the IOP.
    SifInitRpc(0);
    SifInitIopHeap();
    SifLoadFileInit();
    fioInit();

    // Apply the SBV LMB patch to allow modules to be loaded from a buffer in EE RAM.
    sbv_patch_enable_lmb();
}

void binfilecopy(FILE *fi, FILE *fo)
{
    int var;

    while(((var = getc(fi)) != EOF))
    {
        putc(var, fo);
    }
}

int CreateSave(void)
{
    FILE *fi;
    FILE *fo;   

	if(mkdir("mc0:MERITOUS", 0777) < 0) return -1;

	// Write icon.sys file to the memory card.
	// Note: The file was created with my myIconSysGen tool
    fi = fopen("cdrom0:\\ICON.SYS", "r");
    fo = fopen("mc0:/MERITOUS/icon.sys","w");   

    if (fi == NULL)
    {
        printf("\nError opening icon.sys\n");
        return -2;
    } 

    if (fo == NULL)
    {
        printf("\nError creating icon.sys\n");  
		fclose(fi);
        return -3;
    }

    binfilecopy(fi, fo);

    fclose(fi);
    fclose(fo);


    fi = fopen("cdrom0:\\MERITOUS.ICN", "r");
    fo = fopen("mc0:/MERITOUS/meritous.icn","w");   

    if (fi == NULL)
    {
        printf("\nError opening MERITOUS.ICN\n");
        return -4;
    } 

    if (fo == NULL)
    {
        printf("\nError creating meritous.icn\n");  
		fclose(fi);
        return -5;
    }

    binfilecopy(fi, fo);

    fclose(fi);
    fclose(fo);
    return 0;
}

void PS2_load_MC()
{
    int ret = 0;
    FILE *fd;

    ret = SifLoadModule("rom0:XSIO2MAN", 0, NULL);
    if (ret < 0) {
        exit(-1);
    }

    ret = SifLoadModule("rom0:XMCMAN", 0, NULL);
    if (ret < 0) {
        exit(-1);
    }
    ret = SifLoadModule("rom0:XMCSERV", 0, NULL);
    if (ret < 0) {
        printf("Failed to load module: XMCSERV");
    } 

	fd = fopen("mc0:/MERITOUS/icon.sys", "r");
	if(fd ==NULL) 
	{
		printf("\nNo previous save exists, creating...\n");
		if((ret = CreateSave()) < 0) 
			printf("Failed to create save! Errorno: %d\n",ret);
	}
	else
		fclose(fd);
}
