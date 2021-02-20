<script>
	import { onMount } from "svelte";
	import { goto, stores } from "@sapper/app";
	import { mdiLogin, mdiLogout, mdiGithub } from "@mdi/js";

	import "./_app.scss";
	import "./_elevated.scss";

	import TopAppBar, { Row, Section, Title } from "@smui/top-app-bar";
	import { Scrim, AppContent } from "@smui/drawer";
	import IconButton from "@smui/icon-button";
	// import List, { Item, Text } from "@smui/list";
	import { Icon } from "@smui/common";
	import A from "@smui/common/A.svelte";
	import Drawer from "../components/_Drawer.svelte";

	import { post } from "utils.js";

	const { page, session } = stores();

	async function logout() {
		await post(`auth/logout`);
		$session.user = null;
		localStorage.removeItem("jwt");
		goto("/");
	}

	let mainContent;
	let drawerOpen = false;
	let miniWindow = false;
	// let user = undefined;

	let sections = [
		{
			name: "Create Topic",
			route: "/ask",
			protected: true,
			logout_only: false,
		},
		{
			name: "Browse Topics",
			route: "/topics",
			protected: false,
			logout_only: false,
		},
		{
			name: "Tags",
			route: "/tags",
			protected: false,
			logout_only: false,
		},
		{
			name: "Users",
			route: "/users",
			protected: false,
			logout_only: false,
		},
		{
			name: "Profile",
			route: `/users/${$session.user_id}/${$session.user}`,
			protected: true,
			logout_only: false,
		},
		{
			name: "Register",
			route: "/register",
			protected: false,
			logout_only: true,
		},
	];

	onMount(async () => {
		setMiniWindow;
		const unsubscribe = session.subscribe(async ($session) => {
			if($session.user){
				sections[4].route = `/users/${$session.user_id}/${$session.user}`;
			}
		});
		return unsubscribe;
	});

	function setMiniWindow() {
		miniWindow = window.innerWidth < 720;
	}

	let activeSection = sections.find(
		(section) => "route" in section && section.route === $page.path
	);
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
</script>

<svelte:window on:resize={setMiniWindow} />

<TopAppBar variant="static" class="demo-top-app-bar">
	<Row>
		<Section>
			{#if miniWindow}
				<IconButton
					class="material-icons"
					on:click={() => (drawerOpen = !drawerOpen)}
				>
					menu
				</IconButton>
			{/if}
			<Title
				component={A}
				href="/"
				on:click={() => (activeSection = null)}
				class="mdc-theme--primary"
				style={miniWindow ? "padding-left: 0;" : ""}
			>
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
				title="Source Code"
			>
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
	<Drawer {miniWindow} {sections} {drawerOpen} {pickSection} />
	{#if miniWindow}
		<Scrim />
	{/if}
	<AppContent class="app-content">
		<main
			class="main-content"
			bind:this={mainContent}
			class:mdc-elevation--z4={true}
		>
			<slot />
		</main>
	</AppContent>
</div>
