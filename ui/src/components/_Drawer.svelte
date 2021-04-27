<script>
    import Drawer, { Content } from "@smui/drawer";
    import List, { Item, Text } from "@smui/list";
    import { stores } from "@sapper/app";

    const { page, session } = stores();
    export let miniWindow;
    export let sections;
    export let drawerOpen;
    export let pickSection;    
</script>

<Drawer
    variant={miniWindow ? "modal" : null}
    bind:open={drawerOpen}
    class="demo-drawer mdc-theme--secondary-bg {miniWindow
        ? 'demo-drawer-adjust'
        : ''}"
>
    <Content>
        <List>
            {#each sections as section (section.name)}
                {#if $session.user && !section.logout_only}
                    <Item
                        bind:this={section.component}
                        rel="prefetch"
                        href={"route" in section
                            ? section.route
                            : section.shortcut}
                        on:click={() => pickSection(section)}
                        activated={"route" in section &&
                            section.route === $page.path}
                        title={section.name}
                        style={section.indent
                            ? "margin-left: " + section.indent * 25 + "px;"
                            : ""}
                    >
                        <Text style="color: #4285F4">
                            {section.name}
                        </Text>
                    </Item>
                {:else if !section.protected && !$session.user}
                    <Item
                        bind:this={section.component}
                        href={"route" in section
                            ? section.route
                            : section.shortcut}
                        on:click={() => pickSection(section)}
                        activated={"route" in section &&
                            section.route === $page.path}
                        title={section.name}
                        style={section.indent
                            ? "margin-left: " + section.indent * 25 + "px;"
                            : ""}
                    >
                        <Text style="color: #4285F4">
                            {section.name}
                        </Text>
                    </Item>
                {/if}
            {/each}
        </List>
    </Content>
</Drawer>
