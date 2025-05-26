<script lang="ts">
    export let onClick: () => void;
    export let label: string;
    export let disabled: boolean = false;
</script>

<button class="btn btn-primary inline-flex items-center" on:click={onClick} disabled={disabled} 
    ><slot></slot><span>{label}</span></button
>