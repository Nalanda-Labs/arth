<script>
	import { onMount } from "svelte";
	import { goto, stores } from "@sapper/app";
	import { mdiLogin, mdiLogout, mdiGithub } from "@mdi/js";

	import "./_app.scss";
	import "./_elevated.scss";

	import TopAppBar, { Row, Section, Title } from "@smui/top-app-bar";
	import Drawer, { Content, Scrim, AppContent } from "@smui/drawer";
	import IconButton from "@smui/icon-button";
	import List, { Item, Text } from "@smui/list";
	import { Icon } from "@smui/common";
	import A from "@smui/common/A.svelte";

	import { post } from "utils.js";

	const { page, session } = stores();

	async function logout() {
		await post(`auth/logout`);
		$session.user = null;
		localStorage.removeItem("jwt");
		goto("/");
	}

	// const iframe = $page.path.startsWith("/demo/top-app-bar-iframe");

	let mainContent;
	let miniWindow = false;
	let drawerOpen = false;

	const sections = [
		{
			name: "Create Topic",
			route: "/ask",
			protected: true,
		},
		{
			name: "Browse Topics",
			route: "/topics",
			protected: false,
		},
		{
			name: "Tags",
			route: "/tags",
			protected: false,
		},
		{
			name: "Users",
			route: "/users",
			protected: false,
		},
		{
			name: "Profile",
			route: `/profile/${session.user}`,
			protected: true,
		},
		{
			name: "Register",
			route: "/register",
			protected: false,
		},
	];

	let activeSection = sections.find(
		(section) => "route" in section && section.route === $page.path
	);

	onMount(setMiniWindow);

	function pickSection(section) {
		drawerOpen = false;
		mainContent.scrollTop = 0;

		sections.forEach((section) =>
			section.component.$set({ activated: false })
		);
		section.component.$set({ activated: true });

		activeSection =
			"shortcut" in section
				? sections.find((sec) => sec.route === section.shortcut)
				: section;
	}

	function setMiniWindow() {
		miniWindow = window.innerWidth < 720;
	}
</script>

<svelte:window on:resize={setMiniWindow} />

<TopAppBar variant="static" class="demo-top-app-bar">
	<Row>
		<Section>
			{#if miniWindow}
				<IconButton
					class="material-icons"
					on:click={() => (drawerOpen = !drawerOpen)}>
					menu
				</IconButton>
			{/if}
			<Title
				component={A}
				href="/"
				on:click={() => (activeSection = null)}
				class="mdc-theme--primary"
				style={miniWindow ? 'padding-left: 0;' : ''}>
				Arth
			</Title>
		</Section>
		<Section align="end" toolbar>
			{#if $session.user}
				<IconButton on:click={logout} title="Log Out">
					<Icon>
						<svg style="width:24px;height:24px" viewBox="0 0 24 24">
							<path fill="#000000" d={mdiLogout} />
						</svg>
					</Icon>
				</IconButton>
			{:else}
				<IconButton href="/login" title="Log In">
					<Icon>
						<svg style="width:24px;height:24px" viewBox="0 0 24 24">
							<path fill="#000000" d={mdiLogin} />
						</svg>
					</Icon>
				</IconButton>
			{/if}
			<IconButton
				href="htts://github.com/Nalanda-Labs/arth"
				title="Source Code">
				<Icon>
					<svg style="width:24px;height:24px" viewBox="0 0 24 24">
						<path fill="#000000" d={mdiGithub} />
					</svg>
				</Icon>
			</IconButton>
		</Section>
	</Row>
</TopAppBar>
<div class="drawer-container">
	<Drawer
		variant={miniWindow ? 'modal' : null}
		bind:open={drawerOpen}
		class="demo-drawer mdc-theme--secondary-bg {miniWindow ? 'demo-drawer-adjust' : ''}">
		<Content>
			<List>
				{#each sections as section (section.name)}
					{#if section.protected && $session.user}
						<Item
							bind:this={section.component}
							href={'route' in section ? section.route : section.shortcut}
							on:click={() => pickSection(section)}
							activated={'route' in section && section.route === $page.path}
							title={section.name}
							style={section.indent ? 'margin-left: ' + section.indent * 25 + 'px;' : ''}>
							<Text class="mdc-theme--on-secondary">
								{section.name}
							</Text>
						</Item>
					{:else if !section.protected}
						<Item
							bind:this={section.component}
							href={'route' in section ? section.route : section.shortcut}
							on:click={() => pickSection(section)}
							activated={'route' in section && section.route === $page.path}
							title={section.name}
							style={section.indent ? 'margin-left: ' + section.indent * 25 + 'px;' : ''}>
							<Text class="mdc-theme--on-secondary">
								{section.name}
							</Text>
						</Item>
					{/if}
				{/each}
			</List>
		</Content>
	</Drawer>

	{#if miniWindow}
		<Scrim />
	{/if}
	<AppContent class="app-content">
		<main
			class="main-content"
			bind:this={mainContent}
			class:mdc-elevation--z4={true}>
			<slot />
		</main>
	</AppContent>
</div>
